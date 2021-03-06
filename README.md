# Middleware Projects - Polimi 2021/2022

## Simulation and Analysis of Noise Level

### Team members
 Name and Surname | Personal Code |             Email             | Github Username 
:---: |:-------------:|:-----------------------------:| :---:
 Luca De Martini |   10565376    | luca.demartini@mail.polimi.it | @imDema 
 Arianna Dragoni |   10611044   |   arianna.dragoni@mail.polimi.it  |        @ariannadragonii       
 Luigi Fusco |   10601210    |  luigi1.fusco@mail.polimi.it  |        @luigifusco        


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
## Deployment

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
In our deployment the virtual machine acts as the static gateway for the mobile devices, and should run a `mosquitto`
instance acting as a bridge for the regional mosquitto server deployed on the edge through Docker.

This can be done by running the mosquitto server with the config file provided in `iot/mosquitto.conf`. (Note: the address of the bridged server must edited to the correct one)
```sh
mosquitto -c mosquitto.conf
```
A tunnel connecting the cooja border router
to the network inside the virtual machine can be set by running the following command from the `rpl-border-router` directory.
```sh
make TARGET=cooja connect-router-cooja
```

### Configuration

+ **Gateway**: the address of the regional mosquitto server is configured in the `iot/mosquitto.conf` file
+ **Edge**: the regional points of interests are configured with the `node-red-data/poi.csv` file
+ **Simulation**: the simulation parameters are configured with the `mpi/simulation.toml` file

### Outputs

+ **Kafka**: the processed data analytics and enriched sensor data are published to kafka topics:
    + `sensor-data`: Enriched sensor readings
    + `poi-avg-hour`, `poi-avg-day`, `poi-avg-week`: Moving averages of noise levels for points of interest
    + `poi-top-10.csv`: Top 10 points of interest with the highest level of noise over the last hour
    + `poi-streak`: Streaks of good noise level

+ **`csv/`**: the docker compose deployment includes a `kafka-consumer` which dumps the processed data analytics to csv files on a shared volume
