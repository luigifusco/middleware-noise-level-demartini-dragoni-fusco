FROM maven:3-openjdk-11

WORKDIR /app

COPY pom.xml ./
RUN mvn verify clean --fail-never

COPY src/ ./src/
RUN mvn package

CMD ["java", "-jar", "/app/target/kafka_consumer-1.0-jar-with-dependencies.jar"]