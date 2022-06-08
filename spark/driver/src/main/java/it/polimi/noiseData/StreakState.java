package it.polimi.noiseData;

import java.io.Serializable;
import org.apache.spark.api.java.Optional;

public class StreakState implements Serializable {
    private final float t;
    private long currentValue;
    private long maxValue;
    private long baseTimestamp;


    public Optional<Long> updateState(NoiseData noise) {
        // if the noise is less than t, reset the current value
        if (noise.getNoise() > t) {
            this.currentValue = 0;
            baseTimestamp = noise.getTs();
            // mi salvo il timestamp corrente, che sarebbe quando sono
        } else {
            this.currentValue = noise.getTs() - baseTimestamp;
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
}
