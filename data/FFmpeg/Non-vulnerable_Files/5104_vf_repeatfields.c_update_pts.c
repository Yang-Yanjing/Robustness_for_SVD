static void update_pts(AVFilterLink *link, AVFrame *f, int64_t pts, int fields)
{
    if (av_cmp_q(link->frame_rate, (AVRational){30000, 1001}) == 0 &&
         av_cmp_q(link->time_base, (AVRational){1001, 60000}) <= 0
    ) {
        f->pts = pts + av_rescale_q(fields, (AVRational){1001, 60000}, link->time_base);
    } else
        f->pts = AV_NOPTS_VALUE;
}
