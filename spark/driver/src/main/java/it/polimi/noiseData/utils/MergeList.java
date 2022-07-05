package it.polimi.noiseData.utils;

import java.util.ArrayList;
import java.util.HashSet;

import scala.Tuple2;

// Ordered collection of tuples with unique keys.
public class MergeList<K, V> {
    private final ArrayList<Tuple2<K, V>> list;
    private final HashSet<K> set;

    public MergeList() {
        this.list = new ArrayList<>();
        this.set = new HashSet<>();
    }
  
    // Add an tuple to te list only if no other tuple with the same key is present
    // Returns false and does nothing otherwise
    public boolean tryAdd(Tuple2<K, V> item) {
        boolean isNew = this.set.add(item._1);
        if (isNew) {
            list.add(item);
        }
        return isNew;
    }

    // Get the number of tuples in the list
    public int size() {
        return this.list.size();
    }

    // Get the inner ArrayList
    public ArrayList<Tuple2<K, V>> toList() {
        return this.list;
    }
}
