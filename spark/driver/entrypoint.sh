#!/bin/bash

SPARK_HOME=/spark
SPARK_LOG=/spark/logs

cd /app/

/spark/bin/spark-submit --class "it.polimi.noiseData.DataAnalytics" /app/target/spark_tutorial-1.0-jar-with-dependencies.jar /data/