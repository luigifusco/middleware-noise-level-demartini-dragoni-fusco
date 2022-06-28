package it.polimi.noiseData;

import com.google.gson.Gson;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.common.serialization.StringDeserializer;
import org.apache.spark.SparkConf;
import org.apache.spark.api.java.Optional;
import org.apache.spark.api.java.function.Function3;
import org.apache.spark.streaming.Duration;
import org.apache.spark.streaming.State;
import org.apache.spark.streaming.StateSpec;
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

    private static final int minutes = 200;


    public static void main(String[] args) throws Exception {

        String brokers = "kafka:9092";
        String topic = "poi-data";

        Collection<String> topics = Arrays.asList(topic);

        NoiseData noise = new NoiseData();

        Map<String, Object> kafkaParams = new HashMap<>();
        kafkaParams.put("bootstrap.servers", brokers);
        kafkaParams.put("key.deserializer", StringDeserializer.class);
        kafkaParams.put("value.deserializer", StringDeserializer.class);
        kafkaParams.put("group.id", "use_a_separate_group_id_for_each_stream");
        kafkaParams.put("auto.offset.reset", "latest");
        kafkaParams.put("enable.auto.commit", false);

        SparkConf sparkConf = new SparkConf().setAppName("JavaKafkaIntegration");
        JavaStreamingContext streamingContext = new JavaStreamingContext(sparkConf, new Duration(2000));
        streamingContext.checkpoint("/data");

        JavaInputDStream<ConsumerRecord<String, String>> streamJson =
                KafkaUtils.createDirectStream(
                        streamingContext,
                        LocationStrategies.PreferConsistent(),
                        ConsumerStrategies.<String, String>Subscribe(topics, kafkaParams)
                );

        var streamNoiseData = streamJson.mapToPair(record -> {
            NoiseData data = new Gson().fromJson(record.value(), NoiseData.class);
            return new Tuple2<>(record.key(), data);
        });



        // hourly,daily,and weekly moving average of noise level, for each point of interest;
        // tuples with id of the poi, an integer for the sum and the noise of the poi
        var noises = streamNoiseData.mapValues((n) -> new Tuple2<>(1, n.getNoise()));

        var hourly_sum = noises.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)),
                // new Duration(60 * minutes));
                new Duration(6000));
        // var hourly_avg = hourly_sum.mapValues((a) -> a._2 / a._1);
        var hourly_avg = hourly_sum.map((a) -> new Tuple2<>(a._1, a._2._2 / a._2._1));

        var daily_sum = noises.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)),
                new Duration(24 * 60 * minutes));
        var daily_avg = daily_sum.map((a) -> new Tuple2<>(a._1, a._2._2 / a._2._1));

        var weekly_sum = noises.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)),
                new Duration(7 * 24 * 60 * minutes));
        var weekly_avg = weekly_sum.map((a) -> new Tuple2<>(a._1, a._2._2 / a._2._1));


        var jsonHourly = hourly_avg.map((h) -> new NoiseAverage(h._1, h._2).toJsonString(NoiseAverage.Type.HourlyAverage));
        jsonHourly.print();
        // daily_avg.print();
        // weekly_avg.print();


        //  top 10 points of interest with the highest level of noise over the last hour;
        var noiseStreamList = streamNoiseData.map(n -> {
            var t = new Tuple2<String, Float>(n._2.getId(), n._2.getNoise());
            var l = new ArrayList<Tuple2<String, Float>>();
            l.add(t);
            return l;
        });

        var sortedNoiseStreamList = noiseStreamList
                .reduceByWindow( DataAnalytics::mergeTop10,
                        new Duration(60 * minutes),
                        new Duration(60 * minutes));

        // sortedNoiseStreamList.print();


        // point of interest with the longest streak of good noise level
         var streakValues = streamNoiseData.mapWithState(StateSpec.function(new Function3<String, Optional<NoiseData>, State<StreakState>, Optional<Tuple2<String, Long>>>() {
             @Override
             public Optional<Tuple2<String,Long>> call(String s, Optional<NoiseData> noiseData, State<StreakState> state) throws Exception {
                 int T = 69;
                 if (!state.exists()) {
                     state.update(new StreakState(T));
                 }

                 StreakState streakState = state.get();
                 Optional<Long> result = streakState.updateState(noiseData.get());
                 state.update(streakState);

                 if(result.isPresent())
                     return Optional.of(new Tuple2<>(noiseData.get().getId(), result.get()));
                 else
                     return Optional.empty();
         }}));

         var v = streakValues.filter(Optional::isPresent)
                 .map(Optional::get);

         //v.print();

        streamingContext.start();
        try {
            streamingContext.awaitTermination();
        } catch (final InterruptedException e) {
            e.printStackTrace();
        }

    }


    private static ArrayList<Tuple2<String, Float>> mergeTop10(List<Tuple2<String, Float>> list1, List<Tuple2<String, Float>> list2) {
        int i1 = 0;
        int i2 = 0;

        // merge
        var mergedList = new ArrayList<Tuple2<String, Float>>();
        while (i1 < list1.size() && i2 < list2.size()) {
            if ( list1.get(i1)._2 > list2.get(i2)._2) {
                mergedList.add(list1.get(i1++));
            }
            else {
                mergedList.add(list2.get(i2++));
            }
        }
        while (i1 < list1.size()){
            mergedList.add(list1.get(i1++));
        }
        while (i2 < list2.size()){
            mergedList.add(list2.get(i2++));
        }

        // remove duplicates
        var set = new HashSet<>();
        var finalList = new ArrayList<Tuple2<String, Float>>();
        for(var m : mergedList){
            if(!set.contains(m._1)){
                set.add(m._1);
                finalList.add(m);
            }
            if(finalList.size() == 10)
                break;
        }

        return finalList;
    }
}
