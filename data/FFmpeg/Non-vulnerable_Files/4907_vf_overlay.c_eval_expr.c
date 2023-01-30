static void eval_expr(AVFilterContext *ctx)
{
    OverlayContext *s = ctx->priv;
    s->var_values[VAR_X] = av_expr_eval(s->x_pexpr, s->var_values, NULL);
    s->var_values[VAR_Y] = av_expr_eval(s->y_pexpr, s->var_values, NULL);
    s->var_values[VAR_X] = av_expr_eval(s->x_pexpr, s->var_values, NULL);
    s->x = normalize_xy(s->var_values[VAR_X], s->hsub);
    s->y = normalize_xy(s->var_values[VAR_Y], s->vsub);
}
