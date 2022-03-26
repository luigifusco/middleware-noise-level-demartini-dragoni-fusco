### Raw sensor data
> Mobile -> Gateway

Data from mobile IoT sensors

+ noise
+ reliability
+ position (Optional)
+ ts (of reading)

### Gateway sensor data
> Gateway -> Edge

Data relayed from IoT gateway.
Coming from raw sensor data, with added position if missing

+ noise
+ reliability
+ position
+ ts (of reading)

### PoI noise update
> Edge -> Kafka

+ id_poi
+ noise_avg
+ ts (computed n from readings)

