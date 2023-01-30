static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ZPContext *s = ctx->priv;
    double var_values[VARS_NB], nb_frames, zoom, dx, dy;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(in->format);
    AVFrame *out = NULL;
    int i, k, x, y, w, h, ret = 0;
    var_values[VAR_IN_W]  = var_values[VAR_IW] = in->width;
    var_values[VAR_IN_H]  = var_values[VAR_IH] = in->height;
    var_values[VAR_OUT_W] = var_values[VAR_OW] = s->w;
    var_values[VAR_OUT_H] = var_values[VAR_OH] = s->h;
    var_values[VAR_IN]    = inlink->frame_count + 1;
    var_values[VAR_ON]    = outlink->frame_count + 1;
    var_values[VAR_PX]    = s->x;
    var_values[VAR_PY]    = s->y;
    var_values[VAR_X]     = 0;
    var_values[VAR_Y]     = 0;
    var_values[VAR_PZOOM] = s->prev_zoom;
    var_values[VAR_ZOOM]  = 1;
    var_values[VAR_PDURATION] = s->prev_nb_frames;
    var_values[VAR_A]     = (double) in->width / in->height;
    var_values[VAR_SAR]   = inlink->sample_aspect_ratio.num ?
        (double) inlink->sample_aspect_ratio.num / inlink->sample_aspect_ratio.den : 1;
    var_values[VAR_DAR]   = var_values[VAR_A] * var_values[VAR_SAR];
    var_values[VAR_HSUB]  = 1 << desc->log2_chroma_w;
    var_values[VAR_VSUB]  = 1 << desc->log2_chroma_h;
    if ((ret = av_expr_parse_and_eval(&nb_frames, s->duration_expr_str,
                                      var_names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto fail;
    var_values[VAR_DURATION] = nb_frames;
    for (i = 0; i < nb_frames; i++) {
        int px[4];
        int py[4];
        uint8_t *input[4];
        int64_t pts = av_rescale_q(in->pts, inlink->time_base,
                                   outlink->time_base) + s->frame_count;
        var_values[VAR_TIME] = pts * av_q2d(outlink->time_base);
        var_values[VAR_FRAME] = i;
        var_values[VAR_ON] = outlink->frame_count + 1;
        if ((ret = av_expr_parse_and_eval(&zoom, s->zoom_expr_str,
                                          var_names, var_values,
                                          NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
            goto fail;
        zoom = av_clipd(zoom, 1, 10);
        var_values[VAR_ZOOM] = zoom;
        w = in->width * (1.0 / zoom);
        h = in->height * (1.0 / zoom);
        if ((ret = av_expr_parse_and_eval(&dx, s->x_expr_str,
                                          var_names, var_values,
                                          NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
            goto fail;
        x = dx = av_clipd(dx, 0, FFMAX(in->width - w, 0));
        var_values[VAR_X] = dx;
        x &= ~((1 << desc->log2_chroma_w) - 1);
        if ((ret = av_expr_parse_and_eval(&dy, s->y_expr_str,
                                          var_names, var_values,
                                          NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
            goto fail;
        y = dy = av_clipd(dy, 0, FFMAX(in->height - h, 0));
        var_values[VAR_Y] = dy;
        y &= ~((1 << desc->log2_chroma_h) - 1);
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        px[1] = px[2] = FF_CEIL_RSHIFT(x, desc->log2_chroma_w);
        px[0] = px[3] = x;
        py[1] = py[2] = FF_CEIL_RSHIFT(y, desc->log2_chroma_h);
        py[0] = py[3] = y;
        s->sws = sws_alloc_context();
        if (!s->sws) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        for (k = 0; in->data[k]; k++)
            input[k] = in->data[k] + py[k] * in->linesize[k] + px[k];
        av_opt_set_int(s->sws, "srcw", w, 0);
        av_opt_set_int(s->sws, "srch", h, 0);
        av_opt_set_int(s->sws, "src_format", in->format, 0);
        av_opt_set_int(s->sws, "dstw", outlink->w, 0);
        av_opt_set_int(s->sws, "dsth", outlink->h, 0);
        av_opt_set_int(s->sws, "dst_format", outlink->format, 0);
        av_opt_set_int(s->sws, "sws_flags", SWS_BICUBIC, 0);
        if ((ret = sws_init_context(s->sws, NULL, NULL)) < 0)
            goto fail;
        sws_scale(s->sws, (const uint8_t *const *)&input, in->linesize, 0, h, out->data, out->linesize);
        out->pts = pts;
        s->frame_count++;
        ret = ff_filter_frame(outlink, out);
        if (ret < 0)
            break;
        out = NULL;
        sws_freeContext(s->sws);
        s->sws = NULL;
    }
    s->x = dx;
    s->y = dy;
    s->prev_zoom = zoom;
    s->prev_nb_frames = nb_frames;
fail:
    sws_freeContext(s->sws);
    s->sws = NULL;
    av_frame_free(&out);
    av_frame_free(&in);
    return ret;
}
