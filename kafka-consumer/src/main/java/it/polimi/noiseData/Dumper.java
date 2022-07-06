package it.polimi.noiseData;

import java.util.ArrayList;
import java.util.List;

public class Dumper {
    public static void main(String[] args) {
        String[] topics = {"poi-avg-hour", "poi-avg-day", "poi-avg-week", "poi-top-10", "poi-streak"};
        String broker = System.getenv("KAFKA_BROKER");
        if (broker == null || args.length < 1) {
            System.exit(-1);
        }

        List<Thread> threadList = new ArrayList<>();

        for (String topic : topics) {
            DumperThread dumperThread = new DumperThread(topic, broker, args[0]);
            threadList.add(dumperThread);
            dumperThread.start();
        }

        try {
            for (var thread : threadList) thread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
