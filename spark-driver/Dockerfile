FROM maven:3-openjdk-11

ENV BASE_URL=https://archive.apache.org/dist/spark/
ENV SPARK_VERSION=3.2.1
ENV HADOOP_VERSION=3.2

RUN wget ${BASE_URL}/spark-${SPARK_VERSION}/spark-${SPARK_VERSION}-bin-hadoop${HADOOP_VERSION}.tgz \
    && tar -xvzf spark-${SPARK_VERSION}-bin-hadoop${HADOOP_VERSION}.tgz \
    && mv spark-${SPARK_VERSION}-bin-hadoop${HADOOP_VERSION} /spark \
    && rm spark-${SPARK_VERSION}-bin-hadoop${HADOOP_VERSION}.tgz

WORKDIR /app
COPY src/ pom.xml entrypoint.sh ./

RUN mvn clean package

CMD ["./entrypoint.sh"]