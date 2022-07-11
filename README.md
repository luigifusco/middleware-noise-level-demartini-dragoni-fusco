# Middleware Projects - Polimi 2021/2022

## Simulation and Analysis of Noise Level

### Team members
 Name and Surname | Personal Code |             Email             | Github Username 
:---: |:-------------:|:-----------------------------:| :---:
 Luca De Martini |   10565376    | luca.demartini@mail.polimi.it | @imDema 
 Arianna Dragoni |       x       |               x               |        x        
 Luigi Fusco |   10601210    |  luigi1.fusco@mail.polimi.it  |        @luigifusco        


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

The simulation is saved in the `middleware.csc` file, and is expected to be run in the virtual machine
provided during the course after replacing the `mqtt-demo` and `rpl-border-router` demos with the ones in this repo.
In our deployment the virtual machine acts as the edge, and should run a `mosquitto`
instance acting as a bridge with the one deployed through Docker. This can be done by specifying a configuration file with
```sh
mosquitto -c mosquitto.conf
```
editing the address to connect to.
A tunnel conencting the cooja border router
to the network inside the virtual machine can be set with the command
```sh
make TARGET=cooja connect-router-cooja
```
run inside the `rpl-border-router` folder.

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

The communication microservices and the spark cluster have exposed endpoints that can be used to interact and extent the system

Exposed endpoints:
+ `9093`: Kafka broker
+ `1883`: Mosquitto edge server
+ `1880`: NodeRED edge server
+ `8080, 7077`: Spark master

Connections between microservices are managed through docker networks