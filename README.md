# Middleware Projects - Polimi 2021/2022

## Team members
 Name and Surname | Personal Code  | Email | Github Username 
:---: | :---: | :---: | :---:
 Luca De Martini | 10565376 | luca.demartini@mail.polimi.it | @imDema 
 Arianna Dragoni | x |               x               |        x        
 Luigi Fusco | x |               x               |        x        



## How to run

The project can be executed by compiling and running the individual components, however, due to the distributed nature of the task, the system has been designed to be easily deployable as microservices running in a Docker containerized environment.

Each microservice has a `Dockerfile` definition and a `docker-compose.yml` is provided to deploy the whole set of microservices.

A distributed deployment can be orchestrated by using **Docker Swarm** or by running the needed containers on each machine

To bring up the microservices, install Docker with the Docker compose plugin and run
```sh
docker compose up -d
```

The logs can be accessed with
```sh
docker compose logs
```

The IoT tier can use real devices or run as a Cooja simulation.
The mobile sensors connect to a gateway that acts as an IPv6 border router and hosts a mosquitto server relay.
The mosquitto server relays messages to the closest mosquitto server running on the edge.

TODO: setting up the gateway

### Micoservices

+ Backend communication primitive
    + kafka
    + zookeeper
+ Data analysis
    + spark-driver
    + spark-master
    + spark-worker
+ Simulation
    + mpi
    + mpi-node
+ Edge
    + node-red
    + mosquitto
+ Data consultation example
    + kafka-consumer

### Endpoints

(TODO)

Exposed endpoints:
+ `9093`: Kafka server containing the processed data is exposed
+ `1883`: Mosquitto edge server
+ `1880`: NodeRED edge server
+ `8080, 7077`: Spark master

Connections between microservices are managed through docker networks