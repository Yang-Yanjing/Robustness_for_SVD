static void free_slice(SwsSlice *s)
{
    int i;
    if (s) {
        if (s->should_free_lines)
            free_lines(s);
        for (i = 0; i < 4; ++i) {
            av_freep(&s->plane[i].line);
            s->plane[i].tmp = NULL;
        }
    }
}
