static void set_saturation(EQContext *eq)
{
    int i;
    eq->saturation = av_clipf(av_expr_eval(eq->saturation_pexpr, eq->var_values, eq), 0.0, 3.0);
    for (i = 1; i < 3; i++) {
        eq->param[i].contrast = eq->saturation;
        eq->param[i].lut_clean = 0;
        check_values(&eq->param[i], eq);
    }
}
