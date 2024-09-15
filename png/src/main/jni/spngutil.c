#include "spng.h"
#include "libimagequant.h"
void *spng_encode(size_t *size, FILE *fp, unsigned char *buff, const liq_palette *pal, unsigned int w, unsigned int h, unsigned int color)
{
	spng_ctx *ctx = spng_ctx_new(SPNG_CTX_ENCODER);
	void *png_buf = NULL;
	if (!ctx)
		goto gc2;
	spng_set_option(ctx, SPNG_IMG_COMPRESSION_LEVEL, 9);
	spng_set_option(ctx, SPNG_IMG_MEM_LEVEL, 9);
	if (!fp)
		spng_set_option(ctx, SPNG_ENCODE_TO_BUFFER, 1);
	else
		spng_set_png_file(ctx, fp);
	struct spng_ihdr ihdr = {w, h, 8, SPNG_COLOR_TYPE_INDEXED, 0, 0, 0};
	spng_set_ihdr(ctx, &ihdr);
	if (color > 888)
	{
		struct spng_plte ple;
		struct spng_trns trns;
		unsigned char map[256];
		struct spng_plte_entry *pale = (struct spng_plte_entry *)pal->entries;
		unsigned int num = pal->count;
		ple.n_entries = num;
		unsigned char *alps = trns.type3_alpha;
		struct spng_plte_entry *pleen = ple.entries;
		unsigned int ts = 0;
		for (unsigned int i= 0; i < num; i++)
		{
			struct spng_plte_entry lc = pale[i];
			uint8_t alp = lc.alpha;
			if (alp < 255)
			{
				map[i] = ts;
				pleen[ts] = lc;
				alps[ts++] = alp;
			}
		}
		trns.n_type3_entries = ts;
		for (unsigned int i = 0; i < num; i++)
		{
			struct spng_plte_entry lc = pale[i];
			if (lc.alpha >= 255)
			{
				map[i] = ts;
				pleen[ts++] = lc;
			}
		}
		spng_set_plte(ctx, &ple);
		if (trns.n_type3_entries)
			spng_set_trns(ctx, &trns);
			//如果不设置调色板的话，它不生效
		for (unsigned int i = 0, num = w * h; i < num; i++)
		{
			buff[i] = map[buff[i]];
		}
	}
	else
		spng_set_plte(ctx, (struct spng_plte *)pal);
	if(spng_encode_image(ctx, buff, w * h, SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE))
		goto gc;
	if (!fp)
	{
		int ret;
		png_buf = spng_get_png_buffer(ctx, size, &ret);
	}
	else
		png_buf = (void *)1;
gc:
	spng_ctx_free(ctx);
gc2:
	if (fp)
		fclose(fp);
	return png_buf;
}
void *spng_decode(const char *file, unsigned int *width, unsigned int *height)
{
	spng_ctx *ctx;
	ctx = spng_ctx_new(SPNG_CTX_IGNORE_ADLER32);
	if (!ctx)
		return NULL;
	spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
	FILE *fp = fopen(file, "rb+");
	if (!fp)
		return NULL;
	void *image = NULL;
	spng_set_png_file(ctx, fp);
	struct spng_ihdr ihdr;
	if (spng_get_ihdr(ctx, &ihdr))
		goto gc;
	size_t size;
	if (spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &size))
		goto gc;
	image = malloc(size);
	if (image == NULL)
		goto gc;
	if (spng_decode_image(ctx, image, size, SPNG_FMT_RGBA8, SPNG_DECODE_TRNS))
	{
		free(image);
		image = NULL;
		goto gc;
	}
	*width = ihdr.width;
	*height = ihdr.height;
gc:
	fclose(fp);
	spng_ctx_free(ctx);
	return image;
}