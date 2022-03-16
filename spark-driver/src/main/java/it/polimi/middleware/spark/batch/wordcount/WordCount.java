package it.polimi.middleware.spark.batch.wordcount;

import java.util.Arrays;
import java.util.List;

import org.apache.spark.SparkConf;
import org.apache.spark.api.java.JavaPairRDD;
import org.apache.spark.api.java.JavaRDD;
import org.apache.spark.api.java.JavaSparkContext;

import it.polimi.middleware.spark.utils.LogUtils;
import org.apache.spark.sql.SparkSession;
import scala.Tuple2;

public class WordCount {

    public static void main(String [] args) {
        LogUtils.setLogLevel();

        final String filePath = args.length > 0 ? args[0] : "./";
        final String master = args.length > 1 ? args[1] : "spark://localhost:7077";

        SparkConf conf = new SparkConf()
                .setMaster(master)
                .setAppName("WordCount");

        SparkSession spark = SparkSession
                .builder()
                .config(conf)
                .getOrCreate();

        System.out.println(spark.version() + "\n" + scala.util.Properties.versionMsg() + "\n" + System.getProperty("java.version"));

        System.out.println(filePath + "files/wordcount/in.txt");

        JavaRDD<String> lines = spark.sparkContext().broadcast(spark.read().textFile(filePath + "files/wordcount/in.txt").javaRDD());
        JavaRDD<String> words = lines.flatMap(s -> Arrays.asList(s.split(" ")).iterator());
        JavaPairRDD<String, Integer> ones = words.mapToPair(s -> new Tuple2<>(s, 1));
        JavaPairRDD<String, Integer> counts = ones.reduceByKey((i1, i2) -> i1 + i2);

        System.out.println(counts.collect());

        spark.close();
    }

}