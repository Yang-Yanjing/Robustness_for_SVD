static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    FrameRateContext *s = ctx->priv;
    int exact;
    ff_dlog(ctx, "config_output()\n");
    ff_dlog(ctx,
           "config_output() input time base:%u/%u (%f)\n",
           ctx->inputs[0]->time_base.num,ctx->inputs[0]->time_base.den,
           av_q2d(ctx->inputs[0]->time_base));
    
    exact = av_reduce(&s->dest_time_base.num, &s->dest_time_base.den,
                      av_gcd((int64_t)s->srce_time_base.num * s->dest_frame_rate.num,
                             (int64_t)s->srce_time_base.den * s->dest_frame_rate.den ),
                      (int64_t)s->srce_time_base.den * s->dest_frame_rate.num, INT_MAX);
    av_log(ctx, AV_LOG_INFO,
           "time base:%u/%u -> %u/%u exact:%d\n",
           s->srce_time_base.num, s->srce_time_base.den,
           s->dest_time_base.num, s->dest_time_base.den, exact);
    if (!exact) {
        av_log(ctx, AV_LOG_WARNING, "Timebase conversion is not exact\n");
    }
    outlink->frame_rate = s->dest_frame_rate;
    outlink->time_base = s->dest_time_base;
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    ff_dlog(ctx,
           "config_output() output time base:%u/%u (%f) w:%d h:%d\n",
           outlink->time_base.num, outlink->time_base.den,
           av_q2d(outlink->time_base),
           outlink->w, outlink->h);
    av_log(ctx, AV_LOG_INFO, "fps -> fps:%u/%u scene score:%f interpolate start:%d end:%d\n",
            s->dest_frame_rate.num, s->dest_frame_rate.den,
            s->scene_score, s->interp_start, s->interp_end);
    return 0;
}
