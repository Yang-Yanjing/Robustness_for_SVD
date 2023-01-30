static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    LutContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    int i, j, plane, direct = 0;
    if (av_frame_is_writable(in)) {
        direct = 1;
        out = in;
    } else {
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    if (s->is_rgb) {
        
        uint8_t *inrow, *outrow, *inrow0, *outrow0;
        const int w = inlink->w;
        const int h = in->height;
        const uint16_t (*tab)[256*256] = (const uint16_t (*)[256*256])s->lut;
        const int in_linesize  =  in->linesize[0];
        const int out_linesize = out->linesize[0];
        const int step = s->step;
        inrow0  = in ->data[0];
        outrow0 = out->data[0];
        for (i = 0; i < h; i ++) {
            inrow  = inrow0;
            outrow = outrow0;
            for (j = 0; j < w; j++) {
                switch (step) {
                case 4:  outrow[3] = tab[3][inrow[3]]; 
                case 3:  outrow[2] = tab[2][inrow[2]]; 
                case 2:  outrow[1] = tab[1][inrow[1]]; 
                default: outrow[0] = tab[0][inrow[0]];
                }
                outrow += step;
                inrow  += step;
            }
            inrow0  += in_linesize;
            outrow0 += out_linesize;
        }
    } else if (s->is_16bit) {
        
        uint16_t *inrow, *outrow;
        for (plane = 0; plane < 4 && in->data[plane] && in->linesize[plane]; plane++) {
            int vsub = plane == 1 || plane == 2 ? s->vsub : 0;
            int hsub = plane == 1 || plane == 2 ? s->hsub : 0;
            int h = FF_CEIL_RSHIFT(inlink->h, vsub);
            int w = FF_CEIL_RSHIFT(inlink->w, hsub);
            const uint16_t *tab = s->lut[plane];
            const int in_linesize  =  in->linesize[plane] / 2;
            const int out_linesize = out->linesize[plane] / 2;
            inrow  = (uint16_t *)in ->data[plane];
            outrow = (uint16_t *)out->data[plane];
            for (i = 0; i < h; i++) {
                for (j = 0; j < w; j++) {
#if HAVE_BIGENDIAN
                    outrow[j] = av_bswap16(tab[av_bswap16(inrow[j])]);
#else
                    outrow[j] = tab[inrow[j]];
#endif
                }
                inrow  += in_linesize;
                outrow += out_linesize;
            }
        }
    } else {
        
        uint8_t *inrow, *outrow;
        for (plane = 0; plane < 4 && in->data[plane] && in->linesize[plane]; plane++) {
            int vsub = plane == 1 || plane == 2 ? s->vsub : 0;
            int hsub = plane == 1 || plane == 2 ? s->hsub : 0;
            int h = FF_CEIL_RSHIFT(inlink->h, vsub);
            int w = FF_CEIL_RSHIFT(inlink->w, hsub);
            const uint16_t *tab = s->lut[plane];
            const int in_linesize  =  in->linesize[plane];
            const int out_linesize = out->linesize[plane];
            inrow  = in ->data[plane];
            outrow = out->data[plane];
            for (i = 0; i < h; i++) {
                for (j = 0; j < w; j++)
                    outrow[j] = tab[inrow[j]];
                inrow  += in_linesize;
                outrow += out_linesize;
            }
        }
    }
    if (!direct)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
