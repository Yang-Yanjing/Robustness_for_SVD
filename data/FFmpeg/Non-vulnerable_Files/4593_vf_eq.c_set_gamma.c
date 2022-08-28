static void set_gamma(EQContext *eq)
{
    int i;
    eq->gamma        = av_clipf(av_expr_eval(eq->gamma_pexpr,        eq->var_values, eq), 0.1, 10.0);
    eq->gamma_r      = av_clipf(av_expr_eval(eq->gamma_r_pexpr,      eq->var_values, eq), 0.1, 10.0);
    eq->gamma_g      = av_clipf(av_expr_eval(eq->gamma_g_pexpr,      eq->var_values, eq), 0.1, 10.0);
    eq->gamma_b      = av_clipf(av_expr_eval(eq->gamma_b_pexpr,      eq->var_values, eq), 0.1, 10.0);
    eq->gamma_weight = av_clipf(av_expr_eval(eq->gamma_weight_pexpr, eq->var_values, eq), 0.0,  1.0);
    eq->param[0].gamma = eq->gamma * eq->gamma_g;
    eq->param[1].gamma = sqrt(eq->gamma_b / eq->gamma_g);
    eq->param[2].gamma = sqrt(eq->gamma_r / eq->gamma_g);
    for (i = 0; i < 3; i++) {
        eq->param[i].gamma_weight = eq->gamma_weight;
        eq->param[i].lut_clean = 0;
        check_values(&eq->param[i], eq);
    }
}
