#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>

#include "toml/toml.h"
#include "sim.h"
#include "utils.h"
#include "physics.h"
#include "communication.h"
#include "config.h"

MPI_Datatype define_sensor_dt() {
    // float x;
    // float y;
    // double noise_mw;
    sensor_t s;
    MPI_Datatype dt;
    int struct_len = 3;
    int block_lens[struct_len];
    MPI_Datatype types[struct_len];
    // We need to compute the displacement to be really portable
    // (different compilers might align structures differently)
    MPI_Aint displacements[struct_len];
    // Add field
    block_lens[0] = 1;
    types[0] = MPI_FLOAT;
    displacements[0] = (size_t) &(s.x) - (size_t) &s;
    // Add field
    block_lens[1] = 1;
    types[1] = MPI_FLOAT;
    displacements[1] = (size_t) &(s.y) - (size_t) &s;
    // Add field
    block_lens[2] = 1;
    types[2] = MPI_DOUBLE;
    displacements[2] = (size_t) &(s.noise_mw) - (size_t)&s;
    // Create and commit the data structure
    MPI_Type_create_struct(struct_len, block_lens, displacements, types, &dt);
    MPI_Type_commit(&dt);

    return dt;
}

sensor_t* sensor_spawn_n(size_t n, bounds_t bounds) {
  sensor_t* sensors = (sensor_t*)calloc(n, sizeof(sensor_t));

  for (int i = 0; i < n; i++) {
    sensors[i].x = rand_range_f(bounds.x0, bounds.x1);
    sensors[i].y = rand_range_f(bounds.y0, bounds.y1);
    sensors[i].noise_mw = 0.0;
    sensors[i].id = NULL;
  }

  return sensors;
}

void entity_step(entity_t* e, bounds_t bounds, float dt) {
  e->x += e->x_v * dt;
  e->y += e->y_v * dt;

  if (e->x < bounds.x0) {
    e->x = 2 * bounds.x0 - e->x;
    e->x_v *= -1;
  }
  if (e->x >= bounds.x1) {
    e->x = 2 * bounds.x1 - e->x;
    e->x_v *= -1;
  }
  if (e->y < bounds.y0) {
    e->y = 2 * bounds.y0 - e->y;
    e->y_v *= -1;
  }
  if (e->y >= bounds.y1) {
    e->y = 2 * bounds.y1 - e->y;
    e->y_v *= -1;
  }

  // fprintf(stdout, "INFO: %s p: (%5.1f,%5.1f), v: (%5.1f,%5.1f)",
  //   e->kind == PERSON ? "PERSON" : "VEHICHLE",
  //   e->x, e->y,
  //   e->x_v, e->y_v
  // );
}

float entity_noise(const entity_t* e) {
  if (e->kind == PERSON) {
    return dbm_to_lin(NOISE_P);
  } else {
    return dbm_to_lin(NOISE_V);
  }
}

void sensor_add_source(sensor_t *restrict s, const entity_t *restrict e) {
  float d = distance_ang(s->x, s->y, e->x, e->y);
  double contribution = noise_decay(entity_noise(e), d);
  // printf("d: %.1f\tn: %.1f\tc: %.1f\n", d, entity_noise(e), contribution);
  s->noise_mw += contribution;
}

reading_msg_t sensor_get_reading(const sensor_t * s) {
  reading_msg_t r;
  r.id = strdup(s->id);
  r.noise = lin_to_dbm(s->noise_mw);
  r.ts = time(NULL);
  return r;
}

// Process 0 selects a number num.
// All other processes have an array that they filter to only keep the elements
// that are multiples of num.
// Process 0 collects the filtered arrays and print them.
int main(int argc, char** argv) {
  /// DISTRIBUTE SENSORS
  // Init random number generator
  srand(entropy_seed());

  MPI_Init(NULL, NULL);

  MPI_Datatype sensor_dt = define_sensor_dt();

  int my_rank, world_size; 
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Process 0 selects the num
  int n_sensors, n_vehichles, n_people;
  bounds_t bounds;
  float v_p, v_v;
  float t_step;
  sensor_t* sensors = NULL;
  rd_kafka_t* kafka_p = NULL;

  if (my_rank == 0) {
    if (argc != 2) {
      printf("USAGE: sim TOML_CONFIG\n");
      exit(101);
    }

    toml_table_t* conf = config_load(argv[1]);
    toml_table_t* conf_sim = toml_table_get(conf, "simulation");

    t_step = toml_double_get(conf_sim, "time_step");

    toml_table_t* conf_vechichles = toml_table_get(conf_sim, "vehichles");
    toml_table_t* conf_people = toml_table_get(conf_sim, "people");

    int n_p = toml_int_get(conf_people, "number");
    int n_v = toml_int_get(conf_vechichles, "number");
    v_p = toml_double_get(conf_people, "velocity");
    v_v = toml_double_get(conf_vechichles, "velocity");

    n_people = (n_p + world_size - 1) / world_size;
    n_vehichles = (n_v + world_size - 1) / world_size; // TODO: exact division

    bounds.x0 = (float)toml_double_get(conf_sim, "lat_0");
    bounds.x1 = (float)toml_double_get(conf_sim, "lat_1");
    bounds.y0 = (float)toml_double_get(conf_sim, "lon_0");
    bounds.y1 = (float)toml_double_get(conf_sim, "lon_1");

    // TODO: configurable noise
    toml_array_t* conf_sensors = toml_array_get(conf_sim, "sensor");
    for (n_sensors = 0; toml_table_at(conf_sensors, n_sensors); n_sensors++);

    sensors = (sensor_t*)calloc(n_sensors, sizeof(sensor_t));

    for (int i = 0; i < n_sensors; i++) {
      toml_table_t* s = toml_table_at(conf_sensors, i);
      sensors[i].x = toml_double_get(s, "lat");
      sensors[i].y = toml_double_get(s, "lon");
      sensors[i].id = strdup(toml_string_get(s, "id"));
      sensors[i].noise_mw = 0.0;
    }

    toml_table_t* conf_kafka = toml_table_get(conf, "kafka");
    kafka_p = kafka_build_producer(toml_string_get(conf_kafka, "broker"));

    toml_free(conf);
  }

  MPI_Bcast(&n_sensors, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n_people, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n_vehichles, 1, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Bcast(&bounds, 4, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&v_p, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&v_v, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&t_step, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

  printf("%d|\ts: %4d p: %4d v: %4d\n", my_rank, n_sensors, n_people, n_vehichles);

  if (my_rank != 0) {
    sensors = calloc(n_sensors, sizeof(sensor_t));
  }
  MPI_Bcast(sensors, n_sensors, sensor_dt, 0, MPI_COMM_WORLD);
  if (my_rank != 0) {
    for (int j = 0; j < n_sensors; j++) {
      sensors[j].id = NULL; // The pointer which was broadcasted is not valid for the replicas
    }
  }

  int n_entities = n_people + n_vehichles;
  entity_t* entities = (entity_t*)calloc(n_entities, sizeof(entity_t));

  for (int i = 0; i < n_entities; i++) {
    entities[i].x = rand_range_f(bounds.x0, bounds.x1);
    entities[i].y = rand_range_f(bounds.y0, bounds.y1);
    
    float angle = rand_range_f(0, PI2);
    if (i < n_people) {
      entities[i].kind = PERSON;
      entities[i].x_v = v_p * cos(angle) / EARTH_RADIUS;
      entities[i].y_v = v_p * sin(angle) / EARTH_RADIUS;
    } else {
      entities[i].kind = VEHICLE;
      entities[i].x_v = v_v * cos(angle) / EARTH_RADIUS;
      entities[i].y_v = v_v * sin(angle) / EARTH_RADIUS;
    }
  }

  // Simulation loop
  for(int i = 0;; i++) {
    for (int i = 0; i < n_entities; i++) {
      entity_step(&entities[i], bounds, t_step);
    }
    for (int j = 0; j < n_sensors; j++) {
      sensors[j].noise_mw = 0;
      // #pragma omp for reduction(+ : sensors[j].noise_mw)
      for (int i = 0; i < n_entities; i++) {
        sensor_add_source(&sensors[j], &entities[i]);
      }

      double noise_sum;
      MPI_Reduce(
        &sensors[j].noise_mw,
        &noise_sum,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
      );

      if (my_rank == 0) {
        sensors[j].noise_mw = noise_sum;
      }
    }
    if (my_rank == 0) {
      for (int j = 0; j < n_sensors; j++) {
        reading_msg_t r = sensor_get_reading(&sensors[j]);
        kafka_send_reading(kafka_p, &r);
        free(r.id);
      }
      printf("%04d\n", time(NULL));
    }
    sleep(2);
    MPI_Barrier(MPI_COMM_WORLD);
  }

  free(sensors);
  free(entities);

  if (my_rank == 0) {
    kafka_close(kafka_p);
  }

  /// divide the entities counts between nodes
  /// each node spawns them at random positions

  /// update entities positions
  /// each node has the list of simulated sensors
  /// each node measures noise contribution for its entities on all sensors
  /// gather back the sum
  /// master sends back the result
  
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
