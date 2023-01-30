static inline int set_param(AVExpr **pexpr, const char *args, const char *cmd,
                            void (*set_fn)(EQContext *eq), AVFilterContext *ctx)
{
    EQContext *eq = ctx->priv;
    int ret;
    if ((ret = set_expr(pexpr, args, cmd, ctx)) < 0)
        return ret;
    if (eq->eval_mode == EVAL_MODE_INIT)
        set_fn(eq);
    return 0;
}
