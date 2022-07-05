package it.polimi.noiseData;

import java.util.*;

import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerConfig;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.apache.kafka.common.serialization.StringSerializer;


public class KafkaSink {
    private final KafkaProducer<String, String> producer;

    public KafkaSink(String serverAddr) {
        final Properties props = new Properties();
        props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, serverAddr);
        props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
        props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
        this.producer = new KafkaProducer<>(props);
    }


    public void publish(String topic, String key, String value) {
        final ProducerRecord<String, String> record = new ProducerRecord<>(topic, key, value);
        producer.send(record);
    }

    public void close() {
        producer.close();
    }
}