package com.nicdahlquist.pngquant;

import java.io.File;

public class LibPngQuant {

    public static boolean pngQuantFile(File inputFile, File outputFile) {
        //Use default quality in the windows batch file bundled with pngQuant
        return pngQuantFile(inputFile, outputFile,50,100);
    }

    public static boolean pngQuantFile(File inputFile, File outputFile, int minQuality, int maxQuality) {
        //Use lowest speed to get the best quality
        return pngQuantFile(inputFile, outputFile, minQuality, maxQuality, 1);
    }

    public static boolean pngQuantFile(File inputFile, File outputFile, int minQuality, int maxQuality, int speed) {
        //Use the default dither value.
        return pngQuantFile(inputFile, outputFile, minQuality, maxQuality, speed, 1f);
    }

    public static boolean pngQuantFile(File inputFile, File outputFile, int minQuality, int maxQuality, int speed, float floydDitherAmount) {
        String inputFilename = inputFile.getAbsolutePath();
        String outputFilename = outputFile.getAbsolutePath();
        return nativePngQuantFile(inputFilename, outputFilename, minQuality, maxQuality, speed, floydDitherAmount);
    }

    static {
        System.loadLibrary("pngquant");
    }

    private static native boolean nativePngQuantFile(String inputFilename, String outputFilename, int minQuality, int maxQuality, int speed, float floydDitherAmount);

}
