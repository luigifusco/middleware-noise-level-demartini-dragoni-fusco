package it.polimi.noiseData;

import java.io.Serializable;
import org.apache.spark.api.java.Optional;

public class StreakState implements Serializable {
    private final float t;
    private int currentValue;
    private int maxValue;


    public Optional<Integer> updateState(Float noise) {
        // if the noise is less than t, reset the current value
        if (noise < t) {
            this.currentValue = 0;
        }
        else {
            this.currentValue ++;
            if(this.currentValue > maxValue) {
                this.maxValue = this.currentValue;
                return Optional.of(this.maxValue);
            }
        }
        return Optional.empty();
    }

    public StreakState(float t) {
        this.currentValue = 0;
        this.maxValue = 0;
        this.t = t;
    }

    public int getCurrentValue() {
        return currentValue;
    }


    public int getMaxValue() {
        return maxValue;
    }

    public void setCurrentValue(int currentValue) {
        this.currentValue = currentValue;
    }

    public void setMaxValue(int maxValue) {
        this.maxValue = maxValue;
    }
}
