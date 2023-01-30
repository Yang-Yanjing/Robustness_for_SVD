static int
fsdither(TIFF* in, TIFF* out)
{
	unsigned char *outline, *inputline, *inptr;
	short *thisline, *nextline, *tmpptr;
	register unsigned char	*outptr;
	register short *thisptr, *nextptr;
	register uint32 i, j;
	uint32 imax, jmax;
	int lastline, lastpixel;
	int bit;
	tsize_t outlinesize;
	int errcode = 0;
	imax = imagelength - 1;
	jmax = imagewidth - 1;
	inputline = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(in));
	thisline = (short *)_TIFFmalloc(TIFFSafeMultiply(tmsize_t, imagewidth, sizeof (short)));
	nextline = (short *)_TIFFmalloc(TIFFSafeMultiply(tmsize_t, imagewidth, sizeof (short)));
	outlinesize = TIFFScanlineSize(out);
	outline = (unsigned char *) _TIFFmalloc(outlinesize);
	if (! (inputline && thisline && nextline && outline)) {
	    fprintf(stderr, "Out of memory.\n");
	    goto skip_on_error;
	}
	


	if (TIFFReadScanline(in, inputline, 0, 0) <= 0)
            goto skip_on_error;
	inptr = inputline;
	nextptr = nextline;
	for (j = 0; j < imagewidth; ++j)
		*nextptr++ = *inptr++;
	for (i = 1; i < imagelength; ++i) {
		tmpptr = thisline;
		thisline = nextline;
		nextline = tmpptr;
		lastline = (i == imax);
		if (TIFFReadScanline(in, inputline, i, 0) <= 0)
			goto skip_on_error;
		inptr = inputline;
		nextptr = nextline;
		for (j = 0; j < imagewidth; ++j)
			*nextptr++ = *inptr++;
		thisptr = thisline;
		nextptr = nextline;
		_TIFFmemset(outptr = outline, 0, outlinesize);
		bit = 0x80;
		for (j = 0; j < imagewidth; ++j) {
			register int v;
			lastpixel = (j == jmax);
			v = *thisptr++;
			if (v < 0)
				v = 0;
			else if (v > 255)
				v = 255;
			if (v > threshold) {
				*outptr |= bit;
				v -= 255;
			}
			bit >>= 1;
			if (bit == 0) {
				outptr++;
				bit = 0x80;
			}
			if (!lastpixel)
				thisptr[0] += v * 7 / 16;
			if (!lastline) {
				if (j != 0)
					nextptr[-1] += v * 3 / 16;
				*nextptr++ += v * 5 / 16;
				if (!lastpixel)
					nextptr[0] += v / 16;
			}
		}
		if (TIFFWriteScanline(out, outline, i-1, 0) < 0)
			goto skip_on_error;
	}
	goto exit_label;
  skip_on_error:
	errcode = 1;
  exit_label:
	_TIFFfree(inputline);
	_TIFFfree(thisline);
	_TIFFfree(nextline);
	_TIFFfree(outline);
	return errcode;
}