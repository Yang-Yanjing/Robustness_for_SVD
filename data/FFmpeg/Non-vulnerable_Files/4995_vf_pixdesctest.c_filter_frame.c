static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    PixdescTestContext *priv = inlink->dst->priv;
    AVFilterLink *outlink    = inlink->dst->outputs[0];
    AVFrame *out;
    int i, c, w = inlink->w, h = inlink->h;
    const int cw = FF_CEIL_RSHIFT(w, priv->pix_desc->log2_chroma_w);
    const int ch = FF_CEIL_RSHIFT(h, priv->pix_desc->log2_chroma_h);
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    for (i = 0; i < 4; i++) {
        const int h1 = i == 1 || i == 2 ? ch : h;
        if (out->data[i]) {
            uint8_t *data = out->data[i] +
                (out->linesize[i] > 0 ? 0 : out->linesize[i] * (h1-1));
            memset(data, 0, FFABS(out->linesize[i]) * h1);
        }
    }
    
    if (priv->pix_desc->flags & AV_PIX_FMT_FLAG_PAL ||
        priv->pix_desc->flags & AV_PIX_FMT_FLAG_PSEUDOPAL)
        memcpy(out->data[1], in->data[1], AVPALETTE_SIZE);
    for (c = 0; c < priv->pix_desc->nb_components; c++) {
        const int w1 = c == 1 || c == 2 ? cw : w;
        const int h1 = c == 1 || c == 2 ? ch : h;
        for (i = 0; i < h1; i++) {
            av_read_image_line(priv->line,
                               (void*)in->data,
                               in->linesize,
                               priv->pix_desc,
                               0, i, c, w1, 0);
            av_write_image_line(priv->line,
                                out->data,
                                out->linesize,
                                priv->pix_desc,
                                0, i, c, w1);
        }
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
