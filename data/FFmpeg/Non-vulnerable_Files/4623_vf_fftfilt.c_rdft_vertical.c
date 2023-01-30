static void rdft_vertical(FFTFILTContext *fftfilt, int h, int plane)
{
    int i, j;
    fftfilt->rdft = av_rdft_init(fftfilt->rdft_vbits[plane], DFT_R2C);
    for (i = 0; i < fftfilt->rdft_hlen[plane]; i++) {
        for (j = 0; j < h; j++)
            fftfilt->rdft_vdata[plane][i * fftfilt->rdft_vlen[plane] + j] =
            fftfilt->rdft_hdata[plane][j * fftfilt->rdft_hlen[plane] + i];
        copy_rev(fftfilt->rdft_vdata[plane] + i * fftfilt->rdft_vlen[plane], h, fftfilt->rdft_vlen[plane]);
    }
    for (i = 0; i < fftfilt->rdft_hlen[plane]; i++)
        av_rdft_calc(fftfilt->rdft, fftfilt->rdft_vdata[plane] + i * fftfilt->rdft_vlen[plane]);
    av_rdft_end(fftfilt->rdft);
}
