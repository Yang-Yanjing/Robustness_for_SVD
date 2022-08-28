
static int filter_frame(AVFilterLink *inlink, AVFrame *picref)
{
    AVFilterContext *ctx = inlink->dst;
    BlackDetectContext *blackdetect = ctx->priv;
    double picture_black_ratio = 0;
    const uint8_t *p = picref->data[0];
    int x, i;
    for (i = 0; i < inlink->h; i++) {
        for (x = 0; x < inlink->w; x++)
            blackdetect->nb_black_pixels += p[x] <= blackdetect->pixel_black_th_i;
        p += picref->linesize[0];
    }
    picture_black_ratio = (double)blackdetect->nb_black_pixels / (inlink->w * inlink->h);
    av_log(ctx, AV_LOG_DEBUG,
           "frame:%"PRId64" picture_black_ratio:%f pts:%s t:%s type:%c\n",
           inlink->frame_count, picture_black_ratio,
           av_ts2str(picref->pts), av_ts2timestr(picref->pts, &inlink->time_base),
           av_get_picture_type_char(picref->pict_type));
    if (picture_black_ratio >= blackdetect->picture_black_ratio_th) {
        if (!blackdetect->black_started) {
            
            blackdetect->black_started = 1;
            blackdetect->black_start = picref->pts;
            av_dict_set(avpriv_frame_get_metadatap(picref), "lavfi.black_start",
                av_ts2timestr(blackdetect->black_start, &inlink->time_base), 0);
        }
    } else if (blackdetect->black_started) {
        
        blackdetect->black_started = 0;
        blackdetect->black_end = picref->pts;
        check_black_end(ctx);
        av_dict_set(avpriv_frame_get_metadatap(picref), "lavfi.black_end",
            av_ts2timestr(blackdetect->black_end, &inlink->time_base), 0);
    }
    blackdetect->last_picref_pts = picref->pts;
    blackdetect->nb_black_pixels = 0;
    return ff_filter_frame(inlink->dst->outputs[0], picref);
}
