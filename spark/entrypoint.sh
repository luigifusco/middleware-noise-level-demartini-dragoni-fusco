#!/bin/bash

SPARK_HOME=/spark

if [ "$SPARK_MODE" == "master" ]; then
    SPARK_MASTER_HOST=${SPARK_MASTER_HOST:-`hostname`}
    SPARK_MASTER_PORT=7077
    SPARK_MASTER_WEBUI_PORT=8080
    SPARK_MASTER_LOG=/spark/logs

    echo "** Starting Spark in master mode **"
    cd /spark/bin && /spark/sbin/../bin/spark-class org.apache.spark.deploy.master.Master \
        --host $SPARK_MASTER_HOST --port $SPARK_MASTER_PORT --webui-port $SPARK_MASTER_WEBUI_PORT
else
    echo "** Starting Spark in worker mode with master $SPARK_MASTER **"
    spark/sbin/../bin/spark-class org.apache.spark.deploy.worker.Worker $SPARK_MASTER
fi