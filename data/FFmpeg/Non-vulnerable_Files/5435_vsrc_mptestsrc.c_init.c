static av_cold int init(AVFilterContext *ctx)
{
    MPTestContext *test = ctx->priv;
    test->max_pts = test->duration >= 0 ?
        av_rescale_q(test->duration, AV_TIME_BASE_Q, av_inv_q(test->frame_rate)) : -1;
    test->pts = 0;
    av_log(ctx, AV_LOG_VERBOSE, "rate:%d/%d duration:%f\n",
           test->frame_rate.num, test->frame_rate.den,
           test->duration < 0 ? -1 : test->max_pts * av_q2d(av_inv_q(test->frame_rate)));
    init_idct();
    return 0;
}
