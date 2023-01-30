static int process_command(AVFilterContext *ctx, const char *cmd, const char *args,
                           char *res, int res_len, int flags)
{
    EQContext *eq = ctx->priv;
#define SET_PARAM(param_name, set_fn_name)                              \
    if (!strcmp(cmd, #param_name)) return set_param(&eq->param_name##_pexpr, args, cmd, set_##set_fn_name, ctx);
         SET_PARAM(contrast, contrast)
    else SET_PARAM(brightness, brightness)
    else SET_PARAM(saturation, saturation)
    else SET_PARAM(gamma, gamma)
    else SET_PARAM(gamma_r, gamma)
    else SET_PARAM(gamma_g, gamma)
    else SET_PARAM(gamma_b, gamma)
    else SET_PARAM(gamma_weight, gamma)
    else return AVERROR(ENOSYS);
}
