#include <android/bitmap.h>
#include <stdlib.h>
#include "libimagequant.h"
#include "jni.h"
#include <android/log.h>
#define LOG_TAG "pngquant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
//只能是ARGB_8888
JNIEXPORT jbyteArray JNICALL   Java_org_bitmapquant_en(JNIEnv * env, jclass obj,jobject bitmap,jlong attr,jint color,float jfloyd){
    AndroidBitmapInfo info;
    jbyteArray outbytes=NULL;
    liq_attr* liq=(liq_attr*)attr;
    if(!AndroidBitmap_getInfo(env,bitmap,&info)){
    	if(info.format==ANDROID_BITMAP_FORMAT_RGBA_8888){
    	void* pixels;
    	if(!AndroidBitmap_lockPixels(env,bitmap,&pixels)){
    		liq_out_attr outattr={info.width,info.height,jfloyd};
    		outbytes=liq_opt(env,(unsigned char*)pixels,liq,&outattr,color);
    		AndroidBitmap_unlockPixels(env,bitmap);
    	}
    	}
    }
    liq_attr_destroy(liq);
    return outbytes;
}