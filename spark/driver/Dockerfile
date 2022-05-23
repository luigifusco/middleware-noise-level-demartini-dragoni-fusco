FROM maven:3-openjdk-11

ENV BASE_URL=https://archive.apache.org/dist/spark/
ENV SPARK_VERSION=3.2.1
ENV HADOOP_VERSION=3.2

RUN wget ${BASE_URL}/spark-${SPARK_VERSION}/spark-${SPARK_VERSION}-bin-hadoop${HADOOP_VERSION}.tgz \
    && tar -xvzf spark-${SPARK_VERSION}-bin-hadoop${HADOOP_VERSION}.tgz \
    && mv spark-${SPARK_VERSION}-bin-hadoop${HADOOP_VERSION} /spark \
    && rm spark-${SPARK_VERSION}-bin-hadoop${HADOOP_VERSION}.tgz

RUN sed -E "s|^log4j\.rootCategory=INFO|log4j.rootCategory=WARN|" /spark/conf/log4j.properties.template > /spark/conf/log4j.properties

WORKDIR /app

COPY pom.xml ./
RUN mvn verify clean --fail-never

COPY src/ ./src/
RUN mvn package

COPY entrypoint.sh .
CMD ["./entrypoint.sh"]