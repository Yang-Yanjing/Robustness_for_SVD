static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    const AVPixFmtDescriptor *desc;
    FFTFILTContext *fftfilt = ctx->priv;
    AVFrame *out;
    int i, j, plane;
    out = ff_get_video_buffer(outlink, inlink->w, inlink->h);
    if (!out)
        return AVERROR(ENOMEM);
    av_frame_copy_props(out, in);
    desc = av_pix_fmt_desc_get(inlink->format);
    for (plane = 0; plane < desc->nb_components; plane++) {
        int w = inlink->w;
        int h = inlink->h;
        if (plane == 1 || plane == 2) {
            w = FF_CEIL_RSHIFT(w, desc->log2_chroma_w);
            h = FF_CEIL_RSHIFT(h, desc->log2_chroma_h);
        }
        rdft_horizontal(fftfilt, in, w, h, plane);
        rdft_vertical(fftfilt, h, plane);
        
        for (i = 0; i < fftfilt->rdft_hlen[plane]; i++)
            for (j = 0; j < fftfilt->rdft_vlen[plane]; j++)
                fftfilt->rdft_vdata[plane][i * fftfilt->rdft_vlen[plane] + j] *=
                  fftfilt->weight[plane][i * fftfilt->rdft_vlen[plane] + j];
        fftfilt->rdft_vdata[plane][0] += fftfilt->rdft_hlen[plane] * fftfilt->rdft_vlen[plane] * fftfilt->dc[plane];
        irdft_vertical(fftfilt, h, plane);
        irdft_horizontal(fftfilt, out, w, h, plane);
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
