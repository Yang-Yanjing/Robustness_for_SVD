static void check_values(EQParameters *param, EQContext *eq)
{
    if (param->contrast == 1.0 && param->brightness == 0.0 && param->gamma == 1.0)
        param->adjust = NULL;
    else if (param->gamma == 1.0 && fabs(param->contrast) < 7.9)
        param->adjust = eq->process;
    else
        param->adjust = apply_lut;
}
