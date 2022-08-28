static av_always_inline void filter_rgb(FadeContext *s, const AVFrame *frame,
                                        int slice_start, int slice_end,
                                        int do_alpha, int step)
{
    int i, j;
    const uint8_t r_idx  = s->rgba_map[R];
    const uint8_t g_idx  = s->rgba_map[G];
    const uint8_t b_idx  = s->rgba_map[B];
    const uint8_t a_idx  = s->rgba_map[A];
    const uint8_t *c = s->color_rgba;
    for (i = slice_start; i < slice_end; i++) {
        uint8_t *p = frame->data[0] + i * frame->linesize[0];
        for (j = 0; j < frame->width; j++) {
#define INTERP(c_name, c_idx) av_clip_uint8(((c[c_idx]<<16) + ((int)p[c_name] - (int)c[c_idx]) * s->factor + (1<<15)) >> 16)
            p[r_idx] = INTERP(r_idx, 0);
            p[g_idx] = INTERP(g_idx, 1);
            p[b_idx] = INTERP(b_idx, 2);
            if (do_alpha)
                p[a_idx] = INTERP(a_idx, 3);
            p += step;
        }
    }
}
