package it.polimi.noiseData;

import java.io.Serializable;

public class NoiseAverage implements Serializable {
    private String id;
    private Float average;

    public NoiseAverage(String id, Float average) {
        this.id = id;
        this.average = average;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public Float getAverage() {
        return average;
    }

    public void setAverage(Float average) {
        this.average = average;
    }

    public String toJsonString() {
        return "{" +
                "\"id\": \"" + id + "\"," +
                "\"average\": " + average +
                '}';
    }

}



