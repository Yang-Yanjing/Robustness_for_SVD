static int yae_align(AudioFragment *frag,
                     const AudioFragment *prev,
                     const int window,
                     const int delta_max,
                     const int drift,
                     FFTSample *correlation,
                     RDFTContext *complex_to_real)
{
    int       best_offset = -drift;
    FFTSample best_metric = -FLT_MAX;
    FFTSample *xcorr;
    int i0;
    int i1;
    int i;
    yae_xcorr_via_rdft(correlation,
                       complex_to_real,
                       (const FFTComplex *)prev->xdat,
                       (const FFTComplex *)frag->xdat,
                       window);
    
    i0 = FFMAX(window / 2 - delta_max - drift, 0);
    i0 = FFMIN(i0, window);
    i1 = FFMIN(window / 2 + delta_max - drift, window - window / 16);
    i1 = FFMAX(i1, 0);
    
    xcorr = correlation + i0;
    for (i = i0; i < i1; i++, xcorr++) {
        FFTSample metric = *xcorr;
        
        FFTSample drifti = (FFTSample)(drift + i);
        metric *= drifti * (FFTSample)(i - i0) * (FFTSample)(i1 - i);
        if (metric > best_metric) {
            best_metric = metric;
            best_offset = i - window / 2;
        }
    }
    return best_offset;
}
