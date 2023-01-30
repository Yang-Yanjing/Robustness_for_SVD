static void update_lls(LLSModel *m, const double *var)
{
    int i, j;
    for (i = 0; i <= m->indep_count; i++) {
        for (j = i; j <= m->indep_count; j++) {
            m->covariance[i][j] += var[i] * var[j];
        }
    }
}
