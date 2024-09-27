#ifndef PNGQUANT_H
#define PNGQUANT_H

 /* typedefs, common macros, public prototypes */
#include "libimagequant.h"
#include "pngquant_opts.h"

pngquant_error pngquant_file_internal(const char *filename, const char *outname, struct pngquant_options *options, liq_attr *liq);

#endif // PNGQUANT_H
