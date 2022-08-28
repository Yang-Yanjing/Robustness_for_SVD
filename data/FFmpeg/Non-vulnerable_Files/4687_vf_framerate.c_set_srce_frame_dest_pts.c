static void set_srce_frame_dest_pts(AVFilterContext *ctx)
{
    FrameRateContext *s = ctx->priv;
    ff_dlog(ctx, "set_srce_frame_output_pts()\n");
    
    if (s->srce[s->prev])
        s->srce_pts_dest[s->prev] = av_rescale_q(s->srce[s->prev]->pts, s->srce_time_base, s->dest_time_base);
    if (s->srce[s->crnt])
        s->srce_pts_dest[s->crnt] = av_rescale_q(s->srce[s->crnt]->pts, s->srce_time_base, s->dest_time_base);
    if (s->srce[s->next])
        s->srce_pts_dest[s->next] = av_rescale_q(s->srce[s->next]->pts, s->srce_time_base, s->dest_time_base);
}
