package it.polimi.noiseData;

import java.io.Serializable;

public class NoiseAverage implements Serializable {
    private Long time;
    private Float average;

    public NoiseAverage(Float average, Long unix_ts) {
        this.time = unix_ts;
        this.average = average;
    }

    public Long getTime() {
        return time;
    }

    public void setTime(Long time) {
        this.time = time;
    }

    public Float getAverage() {
        return average;
    }

    public void setAverage(Float average) {
        this.average = average;
    }

    public String toJsonString() {
        return "{" +
                "\"time\": \"" + time + "\"," +
                "\"average\": " + average +
                '}';
    }

}



