static void update_alpha(DrawTextContext *s)
{
    double alpha = av_expr_eval(s->a_pexpr, s->var_values, &s->prng);
    if (isnan(alpha))
        return;
    if (alpha >= 1.0)
        s->alpha = 255;
    else if (alpha <= 0)
        s->alpha = 0;
    else
        s->alpha = 256 * alpha;
}
