static void set_contrast(EQContext *eq)
{
    eq->contrast = av_clipf(av_expr_eval(eq->contrast_pexpr, eq->var_values, eq), -1000.0, 1000.0);
    eq->param[0].contrast = eq->contrast;
    eq->param[0].lut_clean = 0;
    check_values(&eq->param[0], eq);
}
