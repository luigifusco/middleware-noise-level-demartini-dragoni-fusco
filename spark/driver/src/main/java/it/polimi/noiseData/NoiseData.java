package it.polimi.noiseData;

import java.io.Serializable;

public class NoiseData implements Serializable {
    public String id;
    public String noise;
    public String ts;

    public NoiseData(){}
    public NoiseData(String id, String noise, String ts) {
        this.id = id;
        this.noise = noise;
        this.ts = ts;
    }

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public String getNoise() { return noise; }
    public void setNoise(String noise) { this.noise = noise; }

    public String getTs() {
        return ts;
    }

    public void setTs(String ts) {
        this.ts = ts;
    }

    @Override
    public String toString() {
        return "NoiseData{" +
                "id='" + id + '\'' +
                ", noise='" + noise + '\'' +
                ", ts='" + ts + '\'' +
                '}';
    }
}
