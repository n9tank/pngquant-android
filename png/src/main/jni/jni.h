
#ifndef _JNI_H
#define _JNI_H

typedef struct liq_out_attr {
    jint w;
    jint h;
    jfloat jfloyd;
} liq_out_attr;
jbyteArray liq_opt(JNIEnv *env, unsigned char *byte, liq_attr *liq, unsigned int w,unsigned h,float jfloyd, unsigned int color);

#endif
