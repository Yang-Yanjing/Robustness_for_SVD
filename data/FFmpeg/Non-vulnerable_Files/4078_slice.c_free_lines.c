static void free_lines(SwsSlice *s)
{
    int i;
    for (i = 0; i < 2; ++i) {
        int n = s->plane[i].available_lines;
        int j;
        for (j = 0; j < n; ++j) {
            av_freep(&s->plane[i].line[j]);
            if (s->is_ring)
               s->plane[i].line[j+n] = NULL;
        }
    }
    for (i = 0; i < 4; ++i)
        memset(s->plane[i].line, 0, sizeof(uint8_t*) * s->plane[i].available_lines * (s->is_ring ? 3 : 1));
    s->should_free_lines = 0;
}
