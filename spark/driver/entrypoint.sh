#!/bin/bash

SPARK_HOME=/spark
SPARK_LOG=/spark/logs

cd /app/

jar tvf /app/target/spark_tutorial-1.0.jar

/spark/bin/spark-submit --class "it.polimi.middleware.spark.batch.wordcount.WordCount" /app/target/spark_tutorial-1.0.jar /data/