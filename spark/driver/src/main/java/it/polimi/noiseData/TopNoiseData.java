package it.polimi.noiseData;

import scala.Tuple2;

import java.io.Serializable;
import java.util.ArrayList;

public class TopNoiseData implements Serializable {
    private  ArrayList<Tuple2<String, Float>> top10List;

    public TopNoiseData(ArrayList<Tuple2<String, Float>> top10List) {
        this.top10List = top10List;
    }

    public String toJsonString() {
        String list = "";
        for (int i = 0; i < top10List.size() - 1; i++) {
            list = list + elementToJsonString(top10List.get(i)._1, top10List.get(i)._2) + ", ";
        }
        list = list + elementToJsonString(top10List.get(top10List.size()- 1)._1, top10List.get(top10List.size()- 1)._2);
        return "[ " + list + " ]";

    }

    private String elementToJsonString(String id, Float noise) {
        return "{" +
                "\"id\": \"" + id + "\"," +
                "\"noise\": " + noise +
                '}';
    }

}
