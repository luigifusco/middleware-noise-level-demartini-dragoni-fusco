package it.polimi.noiseData;

public class StreakJson {
    private String id;
    private Long time;

    public StreakJson(String id, Long time) {
        this.id = id;
        this.time = time;
    }


    public String toJsonString() {
        return "{" +
                "\"id\": \"" + id + "\"," +
                "\"time\": " + time +
                '}';
    }
}
