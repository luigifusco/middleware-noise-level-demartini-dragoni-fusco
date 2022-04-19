#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#include <utils.c>
#include <sim.h>

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


// Process 0 selects a number num.
// All other processes have an array that they filter to only keep the elements
// that are multiples of num.
// Process 0 collects the filtered arrays and print them.
int main(int argc, char** argv) {
  /// DISTRIBUTE SENSORS

  // Init random number generator
  srand(time(NULL));

  MPI_Init(NULL, NULL);

  MPI_Datatype sensor_dt = define_sensor_dt();

  int my_rank, world_size; 
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Process 0 selects the num
  int n_sensors, n_vehichles, n_people;
  sensor_t* sensors;

  if (my_rank == 0) {
    n_sensors = N_SENSORS;
    n_people = N_P;
    n_vehichles = N_V;

    sensors = spawn_sensors(n_sensors, (float)W, (float)H);
  }

  MPI_Bcast(&n_sensors, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n_people, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n_vehichles, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (my_rank != 0) {
    sensors = calloc(n_sensors, sizeof(sensor_t));
  }
  MPI_Bcast(&sensors, n_sensors, sensor_dt, 0, MPI_COMM_WORLD);

  


  /// divide the entities counts between nodes
  /// each node spawns them at random positions

  /// update entities positions
  /// each node has the list of simulated sensors
  /// each node measures noise contribution for its entities on all sensors
  /// gather back the sum
  /// master sends back the result

  MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int* values = create_random_array(num_elements_per_proc, max_val);
  int s = num_elements_per_proc, i = 0;

  while (i < s) {
    if (values[i] % num != 0) {
      swap_remove(values, &s, i);
    } else {
      i += 1;
    }
  }

  int* sizes = NULL;
  int* displacements = NULL;
  int* results = NULL;
  int final_size = 0;

  if (my_rank == 0) {
    sizes = calloc(world_size, sizeof(int));
  }

  MPI_Gather(
    &s, 1, MPI_INT,
    sizes, 1,MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  if (my_rank == 0) {
    displacements = calloc(world_size, sizeof(int));

    for (int i = 0; i < world_size; i++) {
      final_size += sizes[i];
    }
    for (int i = 1; i < world_size; i++) {
      displacements[i] = displacements[i-1] + sizes[i-1];
    }
    
    results = calloc(final_size, sizeof(int));
  }

  MPI_Gatherv(
    values, s, MPI_INT,
    results, sizes, displacements, MPI_INT,
    0,
    MPI_COMM_WORLD
  );

  if (my_rank == 0) {
    for (int i = 0; i < final_size; i++) {
      printf("%d ", results[i]);
    }
    free(sizes);
    free(displacements);
    free(results);
  }

  free(values);
  
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
