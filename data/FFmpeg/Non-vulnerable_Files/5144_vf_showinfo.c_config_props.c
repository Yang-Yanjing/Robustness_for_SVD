static int config_props(AVFilterContext *ctx, AVFilterLink *link, int is_out)
{
    av_log(ctx, AV_LOG_INFO, "config %s time_base: %d/%d, frame_rate: %d/%d\n",
           is_out ? "out" :"in",
           link->time_base.num, link->time_base.den,
           link->frame_rate.num, link->frame_rate.den
    );
    return 0;
}
