package it.polimi.noiseData;

import java.util.*;

import com.google.gson.Gson;
import org.apache.spark.SparkConf;
import org.apache.spark.TaskContext;
import org.apache.spark.api.java.*;
import org.apache.spark.api.java.function.*;
import org.apache.spark.streaming.Duration;
import org.apache.spark.streaming.api.java.*;
import org.apache.spark.streaming.kafka010.*;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.common.TopicPartition;
import org.apache.kafka.common.serialization.StringDeserializer;
import scala.Tuple2;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;


public final class JavaDirectKafka {
    private static final Pattern SPACE = Pattern.compile(" ");

    public static void main(String[] args) throws Exception {

        String brokers = "kafka:9092";
        String topic = "poi-data";

        Collection<String> topics = Arrays.asList(topic);


        Map<String, Object> kafkaParams = new HashMap<>();
        kafkaParams.put("bootstrap.servers", "kafka:9092");
        kafkaParams.put("key.deserializer", StringDeserializer.class);
        kafkaParams.put("value.deserializer", StringDeserializer.class);
        kafkaParams.put("group.id", "use_a_separate_group_id_for_each_stream");
        kafkaParams.put("auto.offset.reset", "latest");
        kafkaParams.put("enable.auto.commit", false);

        SparkConf sparkConf = new SparkConf().setAppName("JavaKafkaIntegration");
        JavaStreamingContext streamingContext = new JavaStreamingContext(sparkConf, new Duration(2000));

        JavaInputDStream<ConsumerRecord<String, String>> streamString =
                KafkaUtils.createDirectStream(
                        streamingContext,
                        LocationStrategies.PreferConsistent(),
                        ConsumerStrategies.<String, String>Subscribe(topics, kafkaParams)
                );


        var streamNoiseData = streamString.mapToPair(record -> {
                NoiseData data = new Gson().fromJson(record.value(), NoiseData.class);
                return new Tuple2<>(record.key(), data);
        });




        streamingContext.start();
        streamingContext.awaitTermination();

    }
}