static void rdft_horizontal(FFTFILTContext *fftfilt, AVFrame *in, int w, int h, int plane)
{
    int i, j;
    fftfilt->rdft = av_rdft_init(fftfilt->rdft_hbits[plane], DFT_R2C);
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++)
            fftfilt->rdft_hdata[plane][i * fftfilt->rdft_hlen[plane] + j] = *(in->data[plane] + in->linesize[plane] * i + j);
        copy_rev(fftfilt->rdft_hdata[plane] + i * fftfilt->rdft_hlen[plane], w, fftfilt->rdft_hlen[plane]);
    }
    for (i = 0; i < h; i++)
        av_rdft_calc(fftfilt->rdft, fftfilt->rdft_hdata[plane] + i * fftfilt->rdft_hlen[plane]);
    av_rdft_end(fftfilt->rdft);
}
