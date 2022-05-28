package it.polimi.noiseData;

import com.google.gson.Gson;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.common.serialization.StringDeserializer;
import org.apache.spark.SparkConf;
import org.apache.spark.api.java.function.Function2;
import org.apache.spark.api.java.function.Function3;
import org.apache.spark.streaming.Duration;
import org.apache.spark.streaming.State;
import org.apache.spark.streaming.api.java.JavaInputDStream;
import org.apache.spark.streaming.api.java.JavaStreamingContext;
import org.apache.spark.streaming.kafka010.ConsumerStrategies;
import org.apache.spark.streaming.kafka010.KafkaUtils;
import org.apache.spark.streaming.kafka010.LocationStrategies;
import scala.Serializable;
import scala.Tuple2;


import java.util.*;
import java.util.regex.Pattern;


public final class DataAnalytics implements Serializable {
    private static final Pattern SPACE = Pattern.compile(" ");

    public static void main(String[] args) throws Exception {

        String brokers = "kafka:9092";
        String topic = "poi-data";

        Collection<String> topics = Arrays.asList(topic);

        NoiseData noise = new NoiseData();

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

        // hourly,daily,and weekly moving average of noise level, for each point of interest;
        // tuples with id of the poi, an integer for the sum and the noise of the poi
        var noises = streamNoiseData.mapValues((n) -> new Tuple2<>(1, n.getNoise()));

        // testing
        var test_sum = noises.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)),
                            new Duration(16000));
        var test_avg = test_sum.map((a) -> new Tuple2<>(a._1, a._2._2 / a._2._1));


        // real functions
//        var hourly_sum = noises.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)),
//                new Duration(3600000));
//        var hourly_avg = hourly_sum.map((a) -> new Tuple2<>(a._1, a._2._2 / a._2._1));
//
//        var daily_sum = noises.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)),
//                new Duration(3600000 * 24));
//        var daily_avg = daily_sum.map((a) -> new Tuple2<>(a._1, a._2._2 / a._2._1));
//
//        var weekly_sum = noises.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)),
//                new Duration(3600000 * 24 * 7));
//        var weekly_avg = weekly_sum.map((a) -> new Tuple2<>(a._1, a._2._2 / a._2._1));

    //    streamNoiseData.print();
     //   test_sum.print();
       // test_avg.print();

       //  top 10 points of interest with the highest level of noise over the last hour;
        // reduce by key and window, e dentro prendo il massimo di ogni noise. poi prendo i noise con i migliori 10 valori
        // con il massimo faccio la reduction:
        var sorted_swapper_hourly_avg = test_avg.mapToPair(Tuple2::swap).transformToPair(s -> s.sortByKey(false));

/*
        var max = streamNoiseData.reduceByKeyAndWindow(new Function2<NoiseData, NoiseData, NoiseData>() {
            @Override
            public NoiseData call(NoiseData noise1, NoiseData noise2){
                if (noise1.getNoise() < noise2.getNoise()) {
                    return noise2;
                }
                return noise1;
            }
        }, new Duration(10000))
                .map(a -> new Tuple2<>(a._2.getId(), a._2.getNoise()))
                .mapToPair(Tuple2::swap)
                .transformToPair(s -> s.sortByKey(false));
*/
        //  max.print();
        var s = streamNoiseData.map(n -> {
            var t = new Tuple2<String, Float>(n._1,n._2.getNoise());
            var l = new ArrayList<Tuple2<String, Float>>();
            l.add(t);
            return l;
        });

        s.print();

        streamingContext.start();



        try {
            streamingContext.awaitTermination();
        } catch (final InterruptedException e) {
            e.printStackTrace();
        }

    }
}
