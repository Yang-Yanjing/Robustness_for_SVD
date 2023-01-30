static double evaluate_lls(LLSModel *m, const double *param, int order)
{
    int i;
    double out = 0;
    for (i = 0; i <= order; i++)
        out += param[i] * m->coeff[order][i];
    return out;
}
