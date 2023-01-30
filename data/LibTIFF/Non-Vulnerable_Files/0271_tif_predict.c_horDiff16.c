static int
horDiff16(TIFF* tif, uint8* cp0, tmsize_t cc)
{
	TIFFPredictorState* sp = PredictorState(tif);
	tmsize_t stride = sp->stride;
	uint16 *wp = (uint16*) cp0;
	tmsize_t wc = cc/2;
    if((cc%(2*stride))!=0)
    {
        TIFFErrorExt(tif->tif_clientdata, "horDiff8",
                     "%s", "(cc%(2*stride))!=0");
        return 0;
    }
	if (wc > stride) {
		wc -= stride;
		wp += wc - 1;
		do {
			REPEAT4(stride, wp[stride] = (uint16)(((unsigned int)wp[stride] - (unsigned int)wp[0]) & 0xffff); wp--)
			wc -= stride;
		} while (wc > 0);
	}
	return 1;
}