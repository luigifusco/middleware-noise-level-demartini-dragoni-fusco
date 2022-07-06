FROM maven:3-openjdk-11

WORKDIR /app

COPY pom.xml ./
RUN mvn verify clean --fail-never

COPY src/ ./src/
RUN mvn package

CMD ["--class "it.polimi.noiseData.DataAnalytics" /app/target/spark_tutorial-1.0-jar-with-dependencies.jar /data/"]