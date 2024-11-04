
#include <jni.h>
#include <stdlib.h>
#include <stdbool.h>
#include "libimagequant.h"
#include "spngutil.h"
static void argb_rgba(unsigned int page, unsigned int *data, unsigned int *drc)
{
	unsigned int index;
	for (index = 0; index < page; index++)
	{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		unsigned int argb = data[index];
		drc[index] = argb << 8 | (argb >> 24);
#else
		unsigned int bgra = data[index];
		unsigned int b0r0 = bgra & 0x00ff00ff;
		drc[index] = (bgra & 0xff00ff00) | (b0r0 << 16) | (b0r0 >> 16);
#endif
	}
}
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
		unsigned int c = ((rgb565 & 0xF800) << 16) | ((rgb565 & 0x1f) << 11);
		rgba[index] = ((c | (c >> 5)) | 0x00FF00FF) & (((g << 13) | (g << 11)) | 0xFF00FFFF);
#else
		unsigned int c = ((rgb565 & 0xF800) >> 8) | ((rgb565 & 0x1f) << 19);
		rgba[index] = ((c | (c >> 5)) | 0xFF00FF00) & (((g << 5) | (g << 3)) | 0xFFFF00FF);
#endif
	}
	return (unsigned char *)rgba;
}
JNIEXPORT jlong JNICALL Java_org_pngquant_attr(JNIEnv *env, jclass obj, jint jMinQuality, jint jMaxQuality, jint jSpeed)
{
	liq_attr *liq = liq_attr_create();
	if (liq == NULL)
	{
		return 0;
	}
	liq_set_quality(liq, jMinQuality, jMaxQuality);
	liq_set_speed(liq, jSpeed);
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
		if (raw_8bit_pixels)
		{
			liq_write_remapped_image(quantization_result, input_image, raw_8bit_pixels, pixels_size);
			const liq_palette *palette = liq_get_palette(quantization_result);
			buf = spng_encode(size, fp, raw_8bit_pixels, palette, w, h, color);
			free(raw_8bit_pixels);
		}
	}
	if (quantization_result)
	{
		liq_result_destroy(quantization_result);
	}
	liq_image_destroy(input_image);
	return buf;
}
jbyteArray tobyte(JNIEnv *env, void *encode, int jsize)
{
	jbyteArray arr = NULL;
	if (encode)
	{
		arr = (*env)->NewByteArray(env, jsize);
		if (arr)
		{
			(*env)->SetByteArrayRegion(env, arr, 0, jsize, encode);
		}
		free(encode);
	}
	return arr;
}
jbyteArray liq_opt(JNIEnv *env, unsigned char *byte, liq_attr *liq, unsigned int w, unsigned h, float jfloyd, unsigned int color)
{
	liq_image *img = liq_image_create_rgba(liq, byte, w, h, 0);
	if (img)
	{
		size_t size;
		void *encode = liq_opt_encode(&size, NULL, img, liq, w, h, jfloyd, color);
		return tobyte(env, encode, size);
	}
	return NULL;
}
JNIEXPORT jbyteArray JNICALL Java_org_pngquant_intEn(JNIEnv *env, jclass obj, jintArray src, jlong attr, jint w, jint h, jfloat jfloyd, jint color, jboolean nosawp)
{
	jbyteArray outbytes = NULL;
	liq_attr *liq = (liq_attr *)attr;
	if (liq)
	{
		jboolean iscopy;
		jint *in = (*env)->GetPrimitiveArrayCritical(env, src, &iscopy);
		if (in)
		{
			unsigned int page = w * h;
			jint *byte;
			iscopy = nosawp & !iscopy;
			if (iscopy)
				byte = malloc(page << 2);
			else
				byte = in;
			void *encode = NULL;
			size_t size = 0;
			if (byte)
			{
				argb_rgba(page, (unsigned int *)in, (unsigned int *)byte);
				if (iscopy)
					(*env)->ReleasePrimitiveArrayCritical(env, src, in, JNI_ABORT);
				liq_image *img = liq_image_create_rgba(liq, byte, w, h, 0);
				if (img)
					encode = liq_opt_encode(&size, NULL, img, liq, w, h, jfloyd, color);
			}
			if (iscopy && byte)
				free(byte);
			else
				(*env)->ReleasePrimitiveArrayCritical(env, src, in, JNI_ABORT);
			outbytes = tobyte(env, encode, size);
		}
		liq_attr_destroy(liq);
	}
	return outbytes;
}
JNIEXPORT jbyteArray JNICALL Java_org_pngquant_shortEn(JNIEnv *env, jclass obj, jshortArray src, jlong attr, jint w, jint h, jfloat jfloyd)
{
	jbyteArray outbytes = NULL;
	liq_attr *liq = (liq_attr *)attr;
	if (liq)
	{
		jshort *in = (*env)->GetPrimitiveArrayCritical(env, src, 0);
		if (in)
		{
			unsigned char *rgba = rgb565_rgba(w * h, (unsigned short *)in);
			(*env)->ReleasePrimitiveArrayCritical(env, src, in, JNI_ABORT);
			if (rgba)
			{
				outbytes = liq_opt(env, rgba, liq, w, h, jfloyd, 565);
				free(rgba);
			}
		}
		liq_attr_destroy(liq);
	}
	return outbytes;
}
JNIEXPORT jboolean JNICALL Java_org_pngquant_file(JNIEnv *env, jclass obj, jstring jInFilename, jstring jOutFilename, jlong attr, jfloat jfloyd)
{
	liq_attr *liq = (liq_attr *)attr;
	bool rest = false;
	if (liq)
	{
		const char *inFilename = (*env)->GetStringUTFChars(env, jInFilename, 0);
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
	}
	return rest;
}
