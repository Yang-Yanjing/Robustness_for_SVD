static void set_work_frame_pts(AVFilterContext *ctx)
{
    FrameRateContext *s = ctx->priv;
    int64_t pts, average_srce_pts_delta = 0;
    ff_dlog(ctx, "set_work_frame_pts()\n");
    av_assert0(s->srce[s->next]);
    av_assert0(s->srce[s->crnt]);
    ff_dlog(ctx, "set_work_frame_pts() srce crnt pts:%"PRId64"\n", s->srce[s->crnt]->pts);
    ff_dlog(ctx, "set_work_frame_pts() srce next pts:%"PRId64"\n", s->srce[s->next]->pts);
    if (s->srce[s->prev])
        ff_dlog(ctx, "set_work_frame_pts() srce prev pts:%"PRId64"\n", s->srce[s->prev]->pts);
    average_srce_pts_delta = s->average_srce_pts_dest_delta;
    ff_dlog(ctx, "set_work_frame_pts() initial average srce pts:%"PRId64"\n", average_srce_pts_delta);
    
    if ((pts = (s->srce[s->next]->pts - s->srce[s->crnt]->pts))) {
        average_srce_pts_delta = average_srce_pts_delta?((average_srce_pts_delta+pts)>>1):pts;
    } else if (s->srce[s->prev] && (pts = (s->srce[s->crnt]->pts - s->srce[s->prev]->pts))) {
        average_srce_pts_delta = average_srce_pts_delta?((average_srce_pts_delta+pts)>>1):pts;
    }
    s->average_srce_pts_dest_delta = av_rescale_q(average_srce_pts_delta, s->srce_time_base, s->dest_time_base);
    ff_dlog(ctx, "set_work_frame_pts() average srce pts:%"PRId64"\n", average_srce_pts_delta);
    ff_dlog(ctx, "set_work_frame_pts() average srce pts:%"PRId64" at dest time base:%u/%u\n",
            s->average_srce_pts_dest_delta, s->dest_time_base.num, s->dest_time_base.den);
    set_srce_frame_dest_pts(ctx);
    if (ctx->inputs[0] && !s->average_dest_pts_delta) {
        int64_t d = av_q2d(av_inv_q(av_mul_q(s->srce_time_base, s->dest_frame_rate)));
        if (d == 0) { 
            av_log(ctx, AV_LOG_WARNING, "Buggy path reached, use settb filter before this filter!\n");
            d = av_q2d(av_mul_q(ctx->inputs[0]->time_base, s->dest_frame_rate));
        }
        s->average_dest_pts_delta = av_rescale_q(d, s->srce_time_base, s->dest_time_base);
        ff_dlog(ctx, "set_frame_pts() average output pts from input timebase\n");
        ff_dlog(ctx, "set_work_frame_pts() average dest pts delta:%"PRId64"\n", s->average_dest_pts_delta);
    }
    if (!s->dest_frame_num) {
        s->pts = s->last_dest_frame_pts = s->srce_pts_dest[s->crnt];
    } else {
        s->pts = s->last_dest_frame_pts + s->average_dest_pts_delta;
    }
    ff_dlog(ctx, "set_work_frame_pts() calculated pts:%"PRId64" at dest time base:%u/%u\n",
            s->pts, s->dest_time_base.num, s->dest_time_base.den);
}
