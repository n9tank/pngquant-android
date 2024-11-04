package org;
import java.util.Arrays;


public class pngquant {
 static{
  System.loadLibrary("pngquant");
 }
 public static final int RGB_565=565;
 public static final int RGB_888=888;
 //该模式因为Android不支持，为平台统一请使用shortEn（RGB_565)
 public static final int ARGB=8888;
 public static native long attr(int jMinQuality, int jMaxQuality, int jSpeed);
 public static native byte[] intEn(int src[], long attr, int w, int h, float fd, int color, boolean nosawp);
 public static native byte[] shortEn(short src[], long attr, int w, int h, float fd);
 public static native boolean file(String in, String out, long attr, float jfloyd);
}
