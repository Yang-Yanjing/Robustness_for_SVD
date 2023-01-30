static void irdft_horizontal(FFTFILTContext *fftfilt, AVFrame *out, int w, int h, int plane)
{
    int i, j;
    fftfilt->rdft = av_rdft_init(fftfilt->rdft_hbits[plane], IDFT_C2R);
    for (i = 0; i < h; i++)
        av_rdft_calc(fftfilt->rdft, fftfilt->rdft_hdata[plane] + i * fftfilt->rdft_hlen[plane]);
    for (i = 0; i < h; i++)
        for (j = 0; j < w; j++)
            *(out->data[plane] + out->linesize[plane] * i + j) = av_clip(fftfilt->rdft_hdata[plane][i
                                                                         *fftfilt->rdft_hlen[plane] + j] * 4 /
                                                                         (fftfilt->rdft_hlen[plane] *
                                                                          fftfilt->rdft_vlen[plane]), 0, 255);
    av_rdft_end(fftfilt->rdft);
}
