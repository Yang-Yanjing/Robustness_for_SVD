static int alloc_lines(SwsSlice *s, int size, int width)
{
    int i;
    int idx[2] = {3, 2};
    s->should_free_lines = 1;
    s->width = width;
    for (i = 0; i < 2; ++i) {
        int n = s->plane[i].available_lines;
        int j;
        int ii = idx[i];
        av_assert0(n == s->plane[ii].available_lines);
        for (j = 0; j < n; ++j) {
            
            
            s->plane[i].line[j] = av_malloc(size * 2 + 32);
            if (!s->plane[i].line[j]) {
                free_lines(s);
                return AVERROR(ENOMEM);
            }
            s->plane[ii].line[j] = s->plane[i].line[j] + size + 16;
            if (s->is_ring) {
               s->plane[i].line[j+n] = s->plane[i].line[j];
               s->plane[ii].line[j+n] = s->plane[ii].line[j];
            }
        }
    }
    return 0;
}
