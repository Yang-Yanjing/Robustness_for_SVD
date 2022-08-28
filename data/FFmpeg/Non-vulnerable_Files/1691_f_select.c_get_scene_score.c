static double get_scene_score(AVFilterContext *ctx, AVFrame *frame)
{
    double ret = 0;
    SelectContext *select = ctx->priv;
    AVFrame *prev_picref = select->prev_picref;
    if (prev_picref &&
        frame->height == prev_picref->height &&
        frame->width  == prev_picref->width) {
        int x, y, nb_sad = 0;
        int64_t sad = 0;
        double mafd, diff;
        uint8_t *p1 =      frame->data[0];
        uint8_t *p2 = prev_picref->data[0];
        const int p1_linesize =       frame->linesize[0];
        const int p2_linesize = prev_picref->linesize[0];
        for (y = 0; y < frame->height - 7; y += 8) {
            for (x = 0; x < frame->width*3 - 7; x += 8) {
                sad += select->sad(p1 + x, p1_linesize, p2 + x, p2_linesize);
                nb_sad += 8 * 8;
            }
            p1 += 8 * p1_linesize;
            p2 += 8 * p2_linesize;
        }
        emms_c();
        mafd = nb_sad ? (double)sad / nb_sad : 0;
        diff = fabs(mafd - select->prev_mafd);
        ret  = av_clipf(FFMIN(mafd, diff) / 100., 0, 1);
        select->prev_mafd = mafd;
        av_frame_free(&prev_picref);
    }
    select->prev_picref = av_frame_clone(frame);
    return ret;
}
