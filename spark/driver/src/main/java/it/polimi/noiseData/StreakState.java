package it.polimi.noiseData;

import java.io.Serializable;
import org.apache.spark.api.java.Optional;

public class StreakState implements Serializable {
    private final float t;
    private long currentValue;
    private long maxValue;
    private long baseTimestamp;


    public Optional<Long> updateState(NoiseData noise) {
        long tsSecs = noise.getTs() / 1000; // We only need second precision for streaks
        // if the noise is higher than t, reset the current value
        if (baseTimestamp == 0){
            baseTimestamp = tsSecs;
        }

        if (noise.getNoise() > t) {
            baseTimestamp = tsSecs;
            // mi salvo il timestamp corrente, che sarebbe quando sono
        } else {
            this.currentValue = tsSecs - baseTimestamp;
            if (this.currentValue > maxValue) {
                this.maxValue = this.currentValue;
                return Optional.of(this.maxValue);
            }
        }
        return Optional.empty();
    }

    public StreakState(float t) {
        this.currentValue = 0;
        this.maxValue = 0;
        this.baseTimestamp = 0;
        this.t = t;
    }

    public float getT() {
        return t;
    }

    public long getCurrentValue() {
        return currentValue;
    }

    public void setCurrentValue(long currentValue) {
        this.currentValue = currentValue;
    }

    public long getMaxValue() {
        return maxValue;
    }

    public void setMaxValue(long maxValue) {
        this.maxValue = maxValue;
    }

    public long getBaseTimestamp() {
        return baseTimestamp;
    }

    public void setBaseTimestamp(long baseTimestamp) {
        this.baseTimestamp = baseTimestamp;
    }

    @Override
    public String toString() {
        return "StreakState [baseTimestamp=" + baseTimestamp + ", currentValue=" + currentValue + ", maxValue="
                + maxValue + ", t=" + t + "]";
    }
}
