# Architecture

+ [ContikiNG] Mobile sensors gather data, may or may not have position
+ [ContikiNG] Gateway forward mobile sensor data to edge, add position info if missing
+ [MPI] Simulation for regions with no sensors, publish simulated readings to Edge
+ [NodeRED] Edge aggregation of noise data to point of interest, publish to kafka stream
+ [Kafka] Communication channel between edge and spark analysis backend
+ [Spark] Data analytics of point of interest noise