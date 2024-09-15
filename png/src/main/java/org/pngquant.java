package org;
import java.nio.ByteBuffer;
import java.security.PublicKey;

public class pngquant {
 static{
  System.loadLibrary("pngquant");
 }
 public static final int RGB_565=565;
 public static final int ARGB=8888;
 public static native long attr(int jMinQuality, int jMaxQuality, int jSpeed);
 public static native long pngAttr(int w, int h, float jfloyd);
 public static native byte[] intEn(int src[], long attr, long attr_out);
 public static native byte[] shortEn(short src[], long attr, long attr_out);
 public static native boolean file(String in, String out, long attr, float jfloyd);
}
