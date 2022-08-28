static int flush_segment(AVFilterContext *ctx)
{
    int ret;
    ConcatContext *cat = ctx->priv;
    unsigned str, str_max;
    int64_t seg_delta;
    find_next_delta_ts(ctx, &seg_delta);
    cat->cur_idx += ctx->nb_outputs;
    cat->nb_in_active = ctx->nb_outputs;
    av_log(ctx, AV_LOG_VERBOSE, "Segment finished at pts=%"PRId64"\n",
           cat->delta_ts);
    if (cat->cur_idx < ctx->nb_inputs) {
        
        str = cat->nb_streams[AVMEDIA_TYPE_VIDEO];
        str_max = str + cat->nb_streams[AVMEDIA_TYPE_AUDIO];
        for (; str < str_max; str++) {
            ret = send_silence(ctx, cat->cur_idx - ctx->nb_outputs + str, str,
                               seg_delta);
            if (ret < 0)
                return ret;
        }
        
        
        str_max = cat->cur_idx + ctx->nb_outputs;
        for (str = cat->cur_idx; str < str_max; str++) {
            while (cat->in[str].queue.available) {
                ret = push_frame(ctx, str, ff_bufqueue_get(&cat->in[str].queue));
                if (ret < 0)
                    return ret;
            }
        }
    }
    return 0;
}
