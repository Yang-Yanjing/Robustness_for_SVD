static int initialize(AVFilterContext *ctx)
{
    EQContext *eq = ctx->priv;
    int ret;
    eq->process = process_c;
    if ((ret = set_expr(&eq->contrast_pexpr,     eq->contrast_expr,     "contrast",     ctx)) < 0 ||
        (ret = set_expr(&eq->brightness_pexpr,   eq->brightness_expr,   "brightness",   ctx)) < 0 ||
        (ret = set_expr(&eq->saturation_pexpr,   eq->saturation_expr,   "saturation",   ctx)) < 0 ||
        (ret = set_expr(&eq->gamma_pexpr,        eq->gamma_expr,        "gamma",        ctx)) < 0 ||
        (ret = set_expr(&eq->gamma_r_pexpr,      eq->gamma_r_expr,      "gamma_r",      ctx)) < 0 ||
        (ret = set_expr(&eq->gamma_g_pexpr,      eq->gamma_g_expr,      "gamma_g",      ctx)) < 0 ||
        (ret = set_expr(&eq->gamma_b_pexpr,      eq->gamma_b_expr,      "gamma_b",      ctx)) < 0 ||
        (ret = set_expr(&eq->gamma_weight_pexpr, eq->gamma_weight_expr, "gamma_weight", ctx)) < 0 )
        return ret;
    if (ARCH_X86)
        ff_eq_init_x86(eq);
    if (eq->eval_mode == EVAL_MODE_INIT) {
        set_gamma(eq);
        set_contrast(eq);
        set_brightness(eq);
        set_saturation(eq);
    }
    return 0;
}
