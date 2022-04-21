#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#include "sim.h"
#include "utils.c"
#include "physics.c"
#include "communication.c"

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
    MPI_Aint current_displacement = 0;
    // Add field
    block_lens[0] = 4;
    types[0] = MPI_FLOAT;
    displacements[0] = (size_t) &(s.x) - (size_t) &s;
    // Add field
    block_lens[1] = 4;
    types[1] = MPI_FLOAT;
    displacements[1] = (size_t) &(s.y) - (size_t) &(s.x);
    // Add field
    block_lens[2] = 8;
    types[2] = MPI_DOUBLE;
    displacements[2] = (size_t) &(s.noise_mw) - (size_t) &(s.y);
    // Create and commit the data structure
    MPI_Type_create_struct(struct_len, block_lens, displacements, types, &dt);
    MPI_Type_commit(&dt);

    return dt;
}

// Creates an array of random numbers.
int *create_random_array(int num_elements, int max_value) {
  int *arr = (int *) malloc(sizeof(int)*num_elements);
  for (int i=0; i<num_elements; i++) {
    arr[i] = (rand() % max_value);
  }
  return arr;
}

sensor_t* spawn_sensors(size_t n, float x_max, float y_max) {
  sensor_t* sensors = (sensor_t*)calloc(n, sizeof(sensor_t));

  for (int i = 0; i < n; i++) {
    sensors[i].x = rand_range_f(0.0, x_max);
    sensors[i].y = rand_range_f(0.0, y_max);
    sensors[i].noise_mw = 0.0;
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
  float d = distance(s->x, s->y, e->x, e->y);
  double contribution = noise_decay(entity_noise(e), d);
  s->noise_mw += contribution;
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
  sensor_t* sensors;

  if (my_rank == 0) {
    n_sensors = N_SENSORS;
    n_people = (N_P + world_size - 1) / world_size;
    n_vehichles = (N_V + world_size - 1) / world_size;

    bounds.x0 = 0.0;
    bounds.x1 = (float)W;
    bounds.y0 = 0.0;
    bounds.y1 = (float)H;

    v_p = V_P;
    v_v = V_V;

    sensors = spawn_sensors(n_sensors, (float)W, (float)H);
  }

  MPI_Bcast(&n_sensors, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n_people, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n_vehichles, 1, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Bcast(&bounds, 4, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&v_p, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&v_v, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);


  if (my_rank != 0) {
    sensors = calloc(n_sensors, sizeof(sensor_t));
  }
  MPI_Bcast(&sensors, n_sensors, sensor_dt, 0, MPI_COMM_WORLD);

  int n_entities = n_people + n_vehichles;
  entity_t* entities = (entity_t*)calloc(n_entities, sizeof(entity_t));

  for (int i = 0; i < n_entities; i++) {
    entities[i].x = rand_range_f(bounds.x0, bounds.x1);
    entities[i].y = rand_range_f(bounds.y0, bounds.y1);
    
    float angle = rand_range_f(0, PI2);
    if (i < n_people) {
      entities[i].kind = PERSON;
      entities[i].x_v = v_p * cos(angle);
      entities[i].y_v = v_p * sin(angle);
    } else {
      entities[i].kind = VEHICLE;
      entities[i].x_v = v_v * cos(angle);
      entities[i].y_v = v_v * sin(angle);
    }
  }

  while(1) {
    for (int i = 0; i < n_entities; i++) {
      entity_step(&entities[i], bounds, TS);
    }
    for (int j = 0; j < n_sensors; j++) {
      sensors[j].noise_mw = 0;
      for (int i = 0; i < n_entities; i++) {
        sensor_add_source(&sensors[j], &entities[i]);
      }

      double noise_sum;
      MPI_Reduce(
        &sensors[j].noise_mw,
        &noise_sum,
        1,
        MPI_FLOAT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
      );

      if (my_rank == 0) {
        sensors[j].noise_mw = noise_sum;
      }
    }
    if (my_rank == 0) {
      send_readings(sensors, n_sensors);
    }
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
