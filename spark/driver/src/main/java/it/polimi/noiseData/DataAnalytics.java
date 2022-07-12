package it.polimi.noiseData;

import com.google.gson.Gson;

import it.polimi.noiseData.utils.MergeList;

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

import java.time.Instant;
import java.util.*;

public final class DataAnalytics implements Serializable {
    private static final int minutes = 200;
    private static final Duration hour = new Duration(60 * minutes);
    private static final Duration day = hour.times(24);
    private static final Duration week = day.times(7);

    public static void main(String[] args) throws Exception {
        String master = System.getenv("SPARK_MASTER");
        String brokers = System.getenv("KAFKA_BROKER");
        String topic = "poi-data";

        if (brokers == null) {
            throw new Exception("Missing KAFKA_BROKER env variable");
        }

        Collection<String> topics = Arrays.asList(topic);

        Map<String, Object> kafkaParams = new HashMap<>();
        kafkaParams.put("bootstrap.servers", brokers);
        kafkaParams.put("key.deserializer", StringDeserializer.class);
        kafkaParams.put("value.deserializer", StringDeserializer.class);
        kafkaParams.put("group.id", "use_a_separate_group_id_for_each_stream");
        kafkaParams.put("auto.offset.reset", "latest");
        kafkaParams.put("enable.auto.commit", false);

        SparkConf sparkConf = new SparkConf()
            .setAppName("NoiseDataAnalytics")
            .setMaster(master);

        JavaStreamingContext streamingContext = new JavaStreamingContext(sparkConf, new Duration(2000));
        streamingContext.checkpoint("/data");

        JavaInputDStream<ConsumerRecord<String, String>> streamJson = KafkaUtils.createDirectStream(
                streamingContext,
                LocationStrategies.PreferConsistent(),
                ConsumerStrategies.<String, String>Subscribe(topics, kafkaParams));

        var noiseData = streamJson.mapToPair(record -> {
            NoiseData data = new Gson().fromJson(record.value(), NoiseData.class);
            return new Tuple2<>(record.key(), data);
        });

        // hourly,daily,and weekly moving average of noise level, for each point of
        // interest;
        // tuples with id of the poi, an integer for the sum and the noise of the poi
        var noiseCount = noiseData.mapValues((n) -> new Tuple2<>(n.getNoise(), 1));

        var avgHour = noiseCount.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)), hour, hour)
                .mapValues((a) -> a._1 / a._2);

        var avgDay = noiseCount.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)), day, day)
                .mapValues((a) -> a._1 / a._2);

        var avgWeek = noiseCount.reduceByKeyAndWindow(((a, b) -> new Tuple2<>(a._1 + b._1, a._2 + b._2)), week, week)
                .mapValues((a) -> a._1 / a._2);

        avgHour.foreachRDD((rdd) -> rdd.foreachPartition(p -> {
            KafkaSink sink = new KafkaSink(brokers);
            Long now = Instant.now().toEpochMilli();
            p.forEachRemaining(t -> sink.publish("poi-avg-hour", t._1, new NoiseAverage(t._2, now).toJsonString()));
        }));

        avgDay.foreachRDD((rdd) -> rdd.foreachPartition(p -> {
            KafkaSink sink = new KafkaSink(brokers);
            Long now = Instant.now().toEpochMilli();
            p.forEachRemaining(t -> sink.publish("poi-avg-day", t._1, new NoiseAverage(t._2, now).toJsonString()));
        }));

        avgWeek.foreachRDD((rdd) -> rdd.foreachPartition(p -> {
            KafkaSink sink = new KafkaSink(brokers);
            Long now = Instant.now().toEpochMilli();
            p.forEachRemaining(t -> sink.publish("poi-avg-week", t._1, new NoiseAverage(t._2, now).toJsonString()));
        }));

        // top 10 points of interest with the highest level of noise over the last hour;
        var noiseDataList = noiseData.map(n -> {
            var t = new Tuple2<String, Float>(n._2.getId(), n._2.getNoise());
            var l = new ArrayList<Tuple2<String, Float>>();
            l.add(t);
            return l;
        });

        var noiseTop10 = noiseDataList
                .reduceByWindow((a, b) -> DataAnalytics.mergeTop(a, b, 10),
                        new Duration(60 * minutes),
                        new Duration(60 * minutes));

        noiseTop10.foreachRDD((rdd) -> rdd.foreachPartition(p -> {
            KafkaSink sink = new KafkaSink(brokers);
            long now = Instant.now().toEpochMilli();
            p.forEachRemaining(t -> sink.publish("poi-top-10", Long.toString(now), new TopNoiseData(t).toJsonString()));
        }));

        // point of interest with the longest streak of good noise level
        var streaks = noiseData.mapWithState(StateSpec.function(
                new Function3<String, Optional<NoiseData>, State<StreakState>, Optional<Tuple2<String, Long>>>() {
                    @Override
                    public Optional<Tuple2<String, Long>> call(String s, Optional<NoiseData> noiseData,
                            State<StreakState> state) throws Exception {
                        if (!noiseData.isPresent()) {
                            return Optional.empty();
                        }

                        int T = 69;
                        if (!state.exists()) {
                            state.update(new StreakState(T));
                        }

                        StreakState streakState = state.get();
                        Optional<Long> result = streakState.updateState(noiseData.get());
                        // System.err.println(noiseData.get().getId() + " " + streakState);
                        state.update(streakState);
                        if (result.isPresent())
                            return Optional.of(new Tuple2<>(noiseData.get().getId(), result.get()));
                        else
                            return Optional.empty();
                    }
                }))
                .filter(Optional::isPresent)
                .mapToPair(Optional::get);

        streaks.foreachRDD((rdd) -> rdd.foreachPartition(p -> {
            KafkaSink sink = new KafkaSink(brokers);
            p.forEachRemaining(t -> sink.publish("poi-streak", t._1, Long.toString(t._2)));
        }));

        streamingContext.start();
        try {
            streamingContext.awaitTermination();
        } catch (final InterruptedException e) {
            e.printStackTrace();
        }

    }

    private static ArrayList<Tuple2<String, Float>> mergeTop(List<Tuple2<String, Float>> list1,
            List<Tuple2<String, Float>> list2, int n) {
        int i1 = 0;
        int i2 = 0;

        var mergeList = new MergeList<String, Float>();

        // Merge sort like strategy
        // Append the highest noise between the heads of the two lists and shift
        while (i1 < list1.size() && i2 < list2.size()) {
            if (list1.get(i1)._2 > list2.get(i2)._2) {
                mergeList.tryAdd(list1.get(i1++));
            } else {
                mergeList.tryAdd(list2.get(i2++));
            }
            // Return if the list is at maximum capacity
            if (mergeList.size() == n) {
                return mergeList.toList();
            }
        }
        // Append the remaining elements
        while (i1 < list1.size()) {
            mergeList.tryAdd(list1.get(i1++));
            // Return if the list is at maximum capacity
            if (mergeList.size() == n) {
                return mergeList.toList();
            }
        }
        while (i2 < list2.size()) {
            mergeList.tryAdd(list2.get(i2++));
            // Return if the list is at maximum capacity
            if (mergeList.size() == n) {
                return mergeList.toList();
            }
        }

        return mergeList.toList();
    }
}
