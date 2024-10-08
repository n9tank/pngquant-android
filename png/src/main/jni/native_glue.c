
#include <jni.h>
#include <android/log.h>

#include <stdlib.h>
#include <stdbool.h>

#include "pngquant/rwpng.h"
#include "pngquant/pngquant.h"

#define  LOG_TAG    "LibPngQuantizer"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void * reserved) {
    return JNI_VERSION_1_6;
}

static void log_callback(const liq_attr * attr, const char * msg, void * user_info) {
    LOGI("%s" ,msg);
}

JNIEXPORT jboolean JNICALL Java_com_nicdahlquist_pngquant_LibPngQuant_nativePngQuantFile(JNIEnv * env, jobject obj, jstring jInFilename, jstring jOutFilename, int jMinQuality, int jMaxQuality, int jSpeed, float jFloyd){
    const char * inFilename = (*env)->GetStringUTFChars(env, jInFilename, 0);
    const char * outFilename = (*env)->GetStringUTFChars(env, jOutFilename, 0);

    struct pngquant_options options = {
        .floyd = jFloyd, // floyd-steinberg dithering
    };
    liq_attr *liq = liq_attr_create();
    options.verbose = true;
    liq_set_quality(liq, jMinQuality, jMaxQuality);
    liq_set_speed(liq, jSpeed);
    liq_set_log_callback(liq, log_callback, NULL);
    options.log_callback = log_callback;

    pngquant_file_internal(inFilename, outFilename, &options, liq);
    
    liq_attr_destroy(liq);
    (*env)->ReleaseStringUTFChars(env, jInFilename, inFilename);
    (*env)->ReleaseStringUTFChars(env, jOutFilename, outFilename);

    return true;
}
