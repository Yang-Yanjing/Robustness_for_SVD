static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ShowVolumeContext *s = ctx->priv;
    int c, i, j, k;
    double values[VAR_VARS_NB];
    if (!s->out || s->out->width  != outlink->w ||
                   s->out->height != outlink->h) {
        av_frame_free(&s->out);
        s->out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!s->out) {
            av_frame_free(&insamples);
            return AVERROR(ENOMEM);
        }
        for (i = 0; i < outlink->h; i++)
            memset(s->out->data[0] + i * s->out->linesize[0], 0, outlink->w * 4);
    }
    s->out->pts = insamples->pts;
    for (j = 0; j < outlink->h; j++) {
        uint8_t *dst = s->out->data[0] + j * s->out->linesize[0];
        for (k = 0; k < s->w; k++) {
            dst[k * 4 + 0] = FFMAX(dst[k * 4 + 0] - s->f, 0);
            dst[k * 4 + 1] = FFMAX(dst[k * 4 + 1] - s->f, 0);
            dst[k * 4 + 2] = FFMAX(dst[k * 4 + 2] - s->f, 0);
            dst[k * 4 + 3] = FFMAX(dst[k * 4 + 3] - s->f, 0);
        }
    }
    for (c = 0; c < inlink->channels; c++) {
        float *src = (float *)insamples->extended_data[c];
        float max = 0;
        uint32_t color;
        for (i = 0; i < insamples->nb_samples; i++)
            max = FFMAX(max, src[i]);
        max = av_clipf(max, 0, 1);
        values[VAR_VOLUME] = 20.0 * log(max) / M_LN10;
        values[VAR_CHANNEL] = c;
        color = av_expr_eval(s->c_expr, values, NULL);
        for (j = 0; j < s->h; j++) {
            uint8_t *dst = s->out->data[0] + (c * s->h + c * s->b + j) * s->out->linesize[0];
            for (k = 0; k < s->w * max; k++)
                AV_WN32A(dst + k * 4, color);
        }
        if (s->h >= 8 && s->draw_text)
            drawtext(s->out, 2, c * (s->h + s->b) + (s->h - 8) / 2,
                     av_get_channel_name(av_channel_layout_extract_channel(insamples->channel_layout, c)));
    }
    av_frame_free(&insamples);
    return ff_filter_frame(outlink, av_frame_clone(s->out));
}
