static int set_expr(AVExpr **pexpr_ptr, char **expr_ptr,
                    const char *expr, const char *option, void *log_ctx)
{
    int ret;
    AVExpr *new_pexpr;
    char *new_expr;
    new_expr = av_strdup(expr);
    if (!new_expr)
        return AVERROR(ENOMEM);
    ret = av_expr_parse(&new_pexpr, expr, var_names,
                        NULL, NULL, NULL, NULL, 0, log_ctx);
    if (ret < 0) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Error when evaluating the expression '%s' for %s\n",
               expr, option);
        av_free(new_expr);
        return ret;
    }
    if (*pexpr_ptr)
        av_expr_free(*pexpr_ptr);
    *pexpr_ptr = new_pexpr;
    av_freep(expr_ptr);
    *expr_ptr = new_expr;
    return 0;
}
