static double get_scene_score(AVFilterContext *ctx, AVFrame *crnt, AVFrame *next)
{
    FrameRateContext *s = ctx->priv;
    double ret = 0;
    ff_dlog(ctx, "get_scene_score()\n");
    if (crnt &&
        crnt->height == next->height &&
        crnt->width  == next->width) {
        int x, y;
        int64_t sad;
        double mafd, diff;
        uint8_t *p1 = crnt->data[0];
        uint8_t *p2 = next->data[0];
        const int p1_linesize = crnt->linesize[0];
        const int p2_linesize = next->linesize[0];
        ff_dlog(ctx, "get_scene_score() process\n");
        for (sad = y = 0; y < crnt->height; y += 8) {
            for (x = 0; x < p1_linesize; x += 8) {
                sad += s->sad(p1 + y * p1_linesize + x,
                              p1_linesize,
                              p2 + y * p2_linesize + x,
                              p2_linesize);
            }
        }
        emms_c();
        mafd = sad / (crnt->height * crnt->width * 3);
        diff = fabs(mafd - s->prev_mafd);
        ret  = av_clipf(FFMIN(mafd, diff), 0, 100.0);
        s->prev_mafd = mafd;
    }
        ff_dlog(ctx, "get_scene_score() result is:%f\n", ret);
    return ret;
}
