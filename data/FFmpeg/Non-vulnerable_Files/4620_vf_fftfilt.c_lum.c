AVFILTER_DEFINE_CLASS(fftfilt);
static inline double lum(void *priv, double x, double y, int plane)
{
    FFTFILTContext *fftfilt = priv;
    return fftfilt->rdft_vdata[plane][(int)x * fftfilt->rdft_vlen[plane] + (int)y];
}
