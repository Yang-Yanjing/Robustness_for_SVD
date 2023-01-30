static int checkmm(AVFilterContext *ctx, int *combs, int m1, int m2,
                   AVFrame **gen_frames, int field)
{
    const FieldMatchContext *fm = ctx->priv;
#define LOAD_COMB(mid) do {                                                     \
    if (combs[mid] < 0) {                                                       \
        if (!gen_frames[mid])                                                   \
            gen_frames[mid] = create_weave_frame(ctx, mid, field,               \
                                                 fm->prv, fm->src, fm->nxt);    \
        combs[mid] = calc_combed_score(fm, gen_frames[mid]);                    \
    }                                                                           \
} while (0)
    LOAD_COMB(m1);
    LOAD_COMB(m2);
    if ((combs[m2] * 3 < combs[m1] || (combs[m2] * 2 < combs[m1] && combs[m1] > fm->combpel)) &&
        abs(combs[m2] - combs[m1]) >= 30 && combs[m2] < fm->combpel)
        return m2;
    else
        return m1;
}
