static uint64
checkAdd64(uint64 summand1, uint64 summand2, T2P* t2p)
{
	uint64 bytes = summand1 + summand2;
	if (bytes < summand1) {
		TIFFError(TIFF2PDF_MODULE, "Integer overflow");
		t2p->t2p_error = T2P_ERR_ERROR;
		bytes = 0;
	}
	return bytes;
}