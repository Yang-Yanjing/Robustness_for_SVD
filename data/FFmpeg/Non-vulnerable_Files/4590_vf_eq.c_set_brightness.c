static void set_brightness(EQContext *eq)
{
    eq->brightness = av_clipf(av_expr_eval(eq->brightness_pexpr, eq->var_values, eq), -1.0, 1.0);
    eq->param[0].brightness = eq->brightness;
    eq->param[0].lut_clean = 0;
    check_values(&eq->param[0], eq);
}
