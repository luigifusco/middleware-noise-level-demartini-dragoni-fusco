```sequence
participant mobile
participant gateway
participant nodered
participant mosquitto
participant spark
participant kafka
participant api
mobile -> gateway: RawSensorData
gateway -> mosquitto: RawSensorData
mosquitto -> nodered: RawSensorData
nodered -> kafka: POINoiseData
api -> kafka: Request
kafka -> spark: Request
kafka -> spark: POINoiseData
spark -> kafka: ProcessedData
kafka -> api: ProcessedData
```

