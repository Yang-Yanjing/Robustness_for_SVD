static av_cold int init(AVFilterContext *ctx)
{
    TestSourceContext *test = ctx->priv;
    test->time_base = av_inv_q(test->frame_rate);
    test->nb_frame = 0;
    test->pts = 0;
    av_log(ctx, AV_LOG_VERBOSE, "size:%dx%d rate:%d/%d duration:%f sar:%d/%d\n",
           test->w, test->h, test->frame_rate.num, test->frame_rate.den,
           test->duration < 0 ? -1 : (double)test->duration/1000000,
           test->sar.num, test->sar.den);
    return 0;
}
