static int filter_frame(AVFilterLink *inlink, AVFrame *inpic)
{
    AVFilterContext   *ctx     = inlink->dst;
    HisteqContext     *histeq  = ctx->priv;
    AVFilterLink      *outlink = ctx->outputs[0];
    int strength  = histeq->strength  * 1000;
    int intensity = histeq->intensity * 1000;
    int x, y, i, luthi, lutlo, lut, luma, oluma, m;
    AVFrame *outpic;
    unsigned int r, g, b, jran;
    uint8_t *src, *dst;
    outpic = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!outpic) {
        av_frame_free(&inpic);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(outpic, inpic);
    
    jran = LCG_SEED;
    

    memset(histeq->in_histogram, 0, sizeof(histeq->in_histogram));
    src = inpic->data[0];
    dst = outpic->data[0];
    for (y = 0; y < inlink->h; y++) {
        for (x = 0; x < inlink->w * histeq->bpp; x += histeq->bpp) {
            GET_RGB_VALUES(r, g, b, src, histeq->rgba_map);
            luma = (55 * r + 182 * g + 19 * b) >> 8;
            dst[x + histeq->rgba_map[A]] = luma;
            histeq->in_histogram[luma]++;
        }
        src += inpic->linesize[0];
        dst += outpic->linesize[0];
    }
#ifdef DEBUG
    for (x = 0; x < 256; x++)
        ff_dlog(ctx, "in[%d]: %u\n", x, histeq->in_histogram[x]);
#endif
    
    histeq->LUT[0] = histeq->in_histogram[0];
    
    for (x = 1; x < 256; x++)
        histeq->LUT[x] = histeq->LUT[x-1] + histeq->in_histogram[x];
    
    for (x = 0; x < 256; x++)
        histeq->LUT[x] = (histeq->LUT[x] * intensity) / (inlink->h * inlink->w);
    

    for (x = 0; x < 256; x++)
        histeq->LUT[x] = (strength * histeq->LUT[x]) / 255 +
                         ((255 - strength) * x)      / 255;
    
    memset(histeq->out_histogram, 0, sizeof(histeq->out_histogram));
    src = inpic->data[0];
    dst = outpic->data[0];
    for (y = 0; y < inlink->h; y++) {
        for (x = 0; x < inlink->w * histeq->bpp; x += histeq->bpp) {
            luma = dst[x + histeq->rgba_map[A]];
            if (luma == 0) {
                for (i = 0; i < histeq->bpp; ++i)
                    dst[x + i] = 0;
                histeq->out_histogram[0]++;
            } else {
                lut = histeq->LUT[luma];
                if (histeq->antibanding != HISTEQ_ANTIBANDING_NONE) {
                    if (luma > 0) {
                        lutlo = histeq->antibanding == HISTEQ_ANTIBANDING_WEAK ?
                                (histeq->LUT[luma] + histeq->LUT[luma - 1]) / 2 :
                                 histeq->LUT[luma - 1];
                    } else
                        lutlo = lut;
                    if (luma < 255) {
                        luthi = (histeq->antibanding == HISTEQ_ANTIBANDING_WEAK) ?
                            (histeq->LUT[luma] + histeq->LUT[luma + 1]) / 2 :
                             histeq->LUT[luma + 1];
                    } else
                        luthi = lut;
                    if (lutlo != luthi) {
                        jran = LCG(jran);
                        lut = lutlo + ((luthi - lutlo + 1) * jran) / LCG_M;
                    }
                }
                GET_RGB_VALUES(r, g, b, src, histeq->rgba_map);
                if (((m = FFMAX3(r, g, b)) * lut) / luma > 255) {
                    r = (r * 255) / m;
                    g = (g * 255) / m;
                    b = (b * 255) / m;
                } else {
                    r = (r * lut) / luma;
                    g = (g * lut) / luma;
                    b = (b * lut) / luma;
                }
                dst[x + histeq->rgba_map[R]] = r;
                dst[x + histeq->rgba_map[G]] = g;
                dst[x + histeq->rgba_map[B]] = b;
                oluma = av_clip_uint8((55 * r + 182 * g + 19 * b) >> 8);
                histeq->out_histogram[oluma]++;
            }
        }
        src += inpic->linesize[0];
        dst += outpic->linesize[0];
    }
#ifdef DEBUG
    for (x = 0; x < 256; x++)
        ff_dlog(ctx, "out[%d]: %u\n", x, histeq->out_histogram[x]);
#endif
    av_frame_free(&inpic);
    return ff_filter_frame(outlink, outpic);
}
