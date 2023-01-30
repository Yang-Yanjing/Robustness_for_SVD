void
TIFFReadRawData(TIFF* tif, int bitrev)
{
	tstrip_t nstrips = TIFFNumberOfStrips(tif);
	const char* what = TIFFIsTiled(tif) ? "Tile" : "Strip";
	uint64* stripbc;
	TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &stripbc);
	if (nstrips > 0) {
		uint32 bufsize = stripbc[0];
		tdata_t buf = _TIFFmalloc(bufsize);
		tstrip_t s;
		for (s = 0; s < nstrips; s++) {
			if (stripbc[s] > bufsize) {
				buf = _TIFFrealloc(buf, stripbc[s]);
				bufsize = stripbc[s];
			}
			if (buf == NULL) {
				fprintf(stderr,
				   "Cannot allocate buffer to read strip %lu\n",
				    (unsigned long) s);
				break;
			}
			if (TIFFReadRawStrip(tif, s, buf, stripbc[s]) < 0) {
				fprintf(stderr, "Error reading strip %lu\n",
				    (unsigned long) s);
				if (stoponerr)
					break;
			} else if (showdata) {
				if (bitrev) {
					TIFFReverseBits(buf, stripbc[s]);
					printf("%s %lu: (bit reversed)\n ",
					    what, (unsigned long) s);
				} else
					printf("%s %lu:\n ", what,
					    (unsigned long) s);
				if (showwords)
					ShowRawWords((uint16*) buf, stripbc[s]>>1);
				else
					ShowRawBytes((unsigned char*) buf, stripbc[s]);
			}
		}
		if (buf != NULL)
			_TIFFfree(buf);
	}
}
