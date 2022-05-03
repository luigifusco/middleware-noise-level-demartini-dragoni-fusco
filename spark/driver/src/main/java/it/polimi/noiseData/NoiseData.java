package it.polimi.noiseData;

public class NoiseData {
    private String id;
    private String noise;
    private String timestamp;

    public NoiseData(){}
    public NoiseData(String id, String noise, String timestamp) {
        this.id = id;
        this.noise = noise;
        this.timestamp = timestamp;
    }

    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public String getNoise() { return noise; }
    public void setNoise(String noise) { this.noise = noise; }

    public String getTimestamp() { return timestamp; }
    public void setTimestamp(String timestamp) { this.timestamp = timestamp; }
}
