```sequence
participant mobile
participant gateway
participant nodered
participant mosquitto
participant spark
participant kafka
participant consumer
mobile -> gateway: RawSensorData
gateway -> mosquitto: RawSensorData
mosquitto -> nodered: RawSensorData
nodered -> kafka: POINoiseData
kafka -> spark: POINoiseData
spark -> kafka: ProcessedData
kafka -> consumer: ProcessedData
```

