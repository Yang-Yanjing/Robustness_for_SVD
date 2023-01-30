static void mul_thrmat_c(int16_t *thr_adr_noq, int16_t *thr_adr, int q)
{
    int a;
    for (a = 0; a < 64; a++)
        thr_adr[a] = q * thr_adr_noq[a];
}
