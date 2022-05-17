package it.polimi.noiseData;

import java.io.Serializable;

public class NoiseData implements Serializable{
    public String id;
    public Float noise;
    public Long ts;

    public NoiseData(){}
    public NoiseData(String id, Float noise, Long ts) {
        this.id = id;
        this.noise = noise;
        this.ts = ts;
    }

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public Float getNoise() { return noise; }
    public void setNoise(Float noise) { this.noise = noise; }

    public Long getTs() {
        return ts;
    }

    public void setTs(Long ts) {
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
