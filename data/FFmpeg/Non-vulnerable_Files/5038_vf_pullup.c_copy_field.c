static void copy_field(PullupContext *s,
                       PullupBuffer *dst, PullupBuffer *src, int parity)
{
    uint8_t *dd, *ss;
    int i;
    for (i = 0; i < s->nb_planes; i++) {
        ss = src->planes[i] + parity * s->planewidth[i];
        dd = dst->planes[i] + parity * s->planewidth[i];
        av_image_copy_plane(dd, s->planewidth[i] << 1,
                            ss, s->planewidth[i] << 1,
                            s->planewidth[i], s->planeheight[i] >> 1);
    }
}
