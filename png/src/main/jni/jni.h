
#ifndef _JNI_H
#define _JNI_H

typedef struct liq_out_attr {
    jint w;
    jint h;
    jfloat jfloyd;
} liq_out_attr;
jbyteArray liq_opt(JNIEnv * env,unsigned char* byte,liq_attr* liq,liq_out_attr* outattr,unsigned int color);

#endif
