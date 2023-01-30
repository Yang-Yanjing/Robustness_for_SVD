static int alloc_slice(SwsSlice *s, enum AVPixelFormat fmt, int lumLines, int chrLines, int h_sub_sample, int v_sub_sample, int ring)
{
    int i;
    int size[4] = { lumLines,
                    chrLines,
                    chrLines,
                    lumLines };
    s->h_chr_sub_sample = h_sub_sample;
    s->v_chr_sub_sample = v_sub_sample;
    s->fmt = fmt;
    s->is_ring = ring;
    s->should_free_lines = 0;
    for (i = 0; i < 4; ++i) {
        int n = size[i] * ( ring == 0 ? 1 : 3);
        s->plane[i].line = av_mallocz_array(sizeof(uint8_t*), n);
        if (!s->plane[i].line)
            return AVERROR(ENOMEM);
        s->plane[i].tmp = ring ? s->plane[i].line + size[i] * 2 : NULL;
        s->plane[i].available_lines = size[i];
        s->plane[i].sliceY = 0;
        s->plane[i].sliceH = 0;
    }
    return 0;
}
