static int config_props(AVFilterLink *inlink)
{
    FFTFILTContext *fftfilt = inlink->dst->priv;
    const AVPixFmtDescriptor *desc;
    int rdft_hbits, rdft_vbits, i, j, plane;
    double values[VAR_VARS_NB];
    desc = av_pix_fmt_desc_get(inlink->format);
    for (i = 0; i < desc->nb_components; i++) {
        int w = inlink->w;
        int h = inlink->h;
        
        for (rdft_hbits = 1; 1 << rdft_hbits < w*10/9; rdft_hbits++);
        fftfilt->rdft_hbits[i] = rdft_hbits;
        fftfilt->rdft_hlen[i] = 1 << rdft_hbits;
        if (!(fftfilt->rdft_hdata[i] = av_malloc_array(h, fftfilt->rdft_hlen[i] * sizeof(FFTSample))))
            return AVERROR(ENOMEM);
        
        for (rdft_vbits = 1; 1 << rdft_vbits < h*10/9; rdft_vbits++);
        fftfilt->rdft_vbits[i] = rdft_vbits;
        fftfilt->rdft_vlen[i] = 1 << rdft_vbits;
        if (!(fftfilt->rdft_vdata[i] = av_malloc_array(fftfilt->rdft_hlen[i], fftfilt->rdft_vlen[i] * sizeof(FFTSample))))
            return AVERROR(ENOMEM);
    }
    
    values[VAR_W] = inlink->w;
    values[VAR_H] = inlink->h;
    for (plane = 0; plane < 3; plane++)
    {
        if(!(fftfilt->weight[plane] = av_malloc_array(fftfilt->rdft_hlen[plane], fftfilt->rdft_vlen[plane] * sizeof(double))))
            return AVERROR(ENOMEM);
        for (i = 0; i < fftfilt->rdft_hlen[plane]; i++)
        {
            values[VAR_X] = i;
            for (j = 0; j < fftfilt->rdft_vlen[plane]; j++)
            {
                values[VAR_Y] = j;
                fftfilt->weight[plane][i * fftfilt->rdft_vlen[plane] + j] =
                av_expr_eval(fftfilt->weight_expr[plane], values, fftfilt);
            }
        }
    }
    return 0;
}
