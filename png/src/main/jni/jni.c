
#include <jni.h>
#include <stdlib.h>
#include <stdbool.h>
#include "libimagequant.h"
#include "spngutil.h"
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
static void argb_rgba(unsigned int page, unsigned int *data)
{
	unsigned int index;
	for (index = 0; index < page; index++)
	{
		unsigned int argb = data[index];
		data[index] = argb << 8 | (argb >> 24);
	}
}
#else
static void bgra_rgba(unsigned int page, unsigned char *data)
{
	unsigned int index;
	for (index = 0; index < page; index += 4)
	{
		unsigned char *p = &data[index];
		uint8_t b = p[0];
		p[0] = p[2];
		p[2] = b;
	}
}
#endif
static unsigned char *rgb565_rgba(unsigned int page, unsigned short *data)
{
	unsigned int *rgba = malloc(page << 2);
	if (rgba == NULL)
		return NULL;
	for (unsigned int index = 0; index < page; index++)
	{
		uint16_t rgb565 = data[index];
		unsigned int g = (rgb565 & 0x7E0);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		unsigned int c = ((rgb565 & 0xF80) << 16) | ((rgb565 & 0x1f) << 11);
		rgba[index] = ((c | (c >> 5)) | 0x00FF00FF) & (((g << 13) | (g << 11)) | 0xFF00FFFF);
#else
		unsigned int c = ((rgb565 & 0xF80) >> 8) | ((rgb565 & 0x1f) << 19);
		rgba[index] = ((c | (c >> 5)) | 0xFF00FF00) & (((g << 5) | (g << 3)) | 0xFFFF00FF);
#endif
	}
	return (unsigned char *)rgba;
}
typedef struct liq_out_attr
{
	jint w;
	jint h;
	jfloat jfloyd;
} liq_out_attr;
JNIEXPORT jlong JNICALL Java_org_pngquant_attr(JNIEnv *env, jclass obj, jint jMinQuality, jint jMaxQuality, jint jSpeed)
{
	liq_attr *liq = liq_attr_create();
	if (liq == NULL)
	{
		(*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/OutOfMemoryError"), NULL);
		return 0;
	}
	liq_set_quality(liq, jMinQuality, jMaxQuality);
	liq_set_speed(liq, jSpeed);
	return (jlong)liq;
}
JNIEXPORT jlong JNICALL Java_org_pngquant_pngAttr(JNIEnv *env, jclass obj, jint w, jint h, jint mode, jfloat jfloyd)
{
	liq_out_attr *liq = malloc(sizeof(liq_out_attr));
	if (liq == NULL)
	{
		(*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/OutOfMemoryError"), NULL);
		return 0;
	}
	liq->jfloyd = jfloyd;
	liq->w = w;
	liq->h = h;
	return (jlong)liq;
}
static void *liq_opt_encode(size_t *size, FILE *fp, liq_image *input_image, liq_attr *liq, unsigned int w, unsigned int h, float jfloyd, unsigned int color)
{
	liq_result *quantization_result = NULL;
	void *buf = NULL;
	if (liq_image_quantize(input_image, liq, &quantization_result) == LIQ_OK)
	{
		liq_set_output_gamma(quantization_result, 0.45455);
		liq_set_dithering_level(quantization_result, jfloyd);
		size_t pixels_size = w * h;
		unsigned char *raw_8bit_pixels = malloc(pixels_size);
		liq_write_remapped_image(quantization_result, input_image, raw_8bit_pixels, pixels_size);
		const liq_palette *palette = liq_get_palette(quantization_result);
		buf = spng_encode(size, fp, raw_8bit_pixels, palette, w, h, color);
		free(raw_8bit_pixels);
	}
	if (quantization_result)
	{
		liq_result_destroy(quantization_result);
	}
	liq_image_destroy(input_image);
	return buf;
}
jbyteArray liq_opt(JNIEnv *env, unsigned char *byte, liq_attr *liq, liq_out_attr *liqout, unsigned int color)
{
	jbyteArray arr = NULL;
	unsigned int w = liqout->w;
	unsigned int h = liqout->h;
	liq_image *img = liq_image_create_rgba(liq, byte, w, h, 0);
	if (img)
	{
		size_t size;
		void *encode = liq_opt_encode(&size, NULL, img, liq, w, h, liqout->jfloyd, color);
		if (encode)
		{
			int jsize = size;
			arr = (*env)->NewByteArray(env, jsize);
			if (arr)
			{
				(*env)->SetByteArrayRegion(env, arr, 0, jsize, (void *)encode);
			}
			free(encode);
		}
	}
	return arr;
}
JNIEXPORT jbyteArray JNICALL Java_org_pngquant_intEn(JNIEnv *env, jclass obj, jintArray src, jlong attr, jlong outattr)
{
	jint *in = (*env)->GetIntArrayElements(env, src, 0);
	if (in == NULL)
		return 0;
	liq_out_attr *out = (liq_out_attr *)outattr;
	unsigned int page = out->w * out->h;
	unsigned char *byte = (unsigned char *)in;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	argb_rgba(page, in);
#else
	bgra_rgba(page << 2, byte);
#endif
	liq_attr *liq = (liq_attr *)attr;
	jbyteArray outbytes = liq_opt(env, byte, liq, out, 8888);
	liq_attr_destroy(liq);
	free(out);
	(*env)->ReleasePrimitiveArrayCritical(env, src, in, JNI_ABORT);
	return outbytes;
}
JNIEXPORT jbyteArray JNICALL Java_org_pngquant_shortEn(JNIEnv *env, jclass obj, jshortArray src, jlong attr, jlong outattr)
{
	jshort *in = (*env)->GetShortArrayElements(env, src, 0);
	if (in == NULL)
		return 0;
	liq_out_attr *out = (liq_out_attr *)outattr;
	unsigned char *rgba = rgb565_rgba(out->w * out->h, (unsigned short *)in);
	(*env)->ReleasePrimitiveArrayCritical(env, src, in, JNI_ABORT);
	//这个接口很鸡肋，建议用intEn
	jbyteArray outbytes = NULL;
	liq_attr *liq = (liq_attr *)attr;
	if (rgba)
	{
		outbytes = liq_opt(env, rgba, liq, out, 565);
		free(rgba);
	}
	liq_attr_destroy(liq);
	free(out);
	return outbytes;
}
JNIEXPORT jboolean JNICALL Java_org_pngquant_file(JNIEnv *env, jclass obj, jstring jInFilename, jstring jOutFilename, jlong attr, jfloat jfloyd)
{
	const char *inFilename = (*env)->GetStringUTFChars(env, jInFilename, 0);
	liq_attr *liq = (liq_attr *)attr;
	bool rest = false;
	if (inFilename)
	{
		unsigned int width, height;
		void *raw_rgba_pixels = spng_decode(inFilename, &width, &height);
		if (raw_rgba_pixels)
		{
			liq_image *input_image = liq_image_create_rgba(liq, raw_rgba_pixels, width, height, 0);
			size_t size;
			const char *outFilename = (*env)->GetStringUTFChars(env, jOutFilename, 0);
			if (outFilename)
			{
				FILE *fp = fopen(outFilename, "wb");
				if (fp)
				{
					if (liq_opt_encode(&size, fp, input_image, liq, width, height, jfloyd, 8888))
						rest = true;
					else
						remove(outFilename);
				}
				(*env)->ReleaseStringUTFChars(env, jOutFilename, outFilename);
			}
		}
		free(raw_rgba_pixels);
		(*env)->ReleaseStringUTFChars(env, jInFilename, inFilename);
	}
	liq_attr_destroy(liq);
	return rest;
}
