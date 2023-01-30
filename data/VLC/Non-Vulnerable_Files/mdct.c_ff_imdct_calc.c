 */
void ff_imdct_calc(MDCTContext *s,
                   int32_t *output,
                   int32_t *input)
{
    int k, n8, n4, n2, n, j,scale;
    const int32_t *tcos = s->tcos;
    const int32_t *tsin = s->tsin;
    const int32_t *in1, *in2;
    FFTComplex *z1 = (FFTComplex *)output;
    FFTComplex *z2 = (FFTComplex *)input;
    int revtabshift = 12 - s->nbits;
    n = 1 << s->nbits;
    n2 = n >> 1;
    n4 = n >> 2;
    n8 = n >> 3;
    /* pre rotation */
    in1 = input;
    in2 = input + n2 - 1;
    for(k = 0; k < n4; k++)
    {
        j=revtab0[k<<revtabshift];
        CMUL(&z1[j].re, &z1[j].im, *in2, *in1, tcos[k], tsin[k]);
        in1 += 2;
        in2 -= 2;
    }
    scale = fft_calc_unscaled(&s->fft, z1);
    /* post rotation + reordering */
    for(k = 0; k < n4; k++)
    {
        CMUL(&z2[k].re, &z2[k].im, (z1[k].re), (z1[k].im), tcos[k], tsin[k]);
    }
    for(k = 0; k < n8; k++)
    {
        int32_t r1,r2,r3,r4,r1n,r2n,r3n;
        r1 = z2[n8 + k].im;
        r1n = r1 * -1;
        r2 = z2[n8-1-k].re;
        r2n = r2 * -1;
        r3 = z2[k+n8].re;
        r3n = r3 * -1;
        r4 = z2[n8-k-1].im;
        output[2*k] = r1n;
        output[n2-1-2*k] = r1;
        output[2*k+1] = r2;
        output[n2-1-2*k-1] = r2n;
        output[n2 + 2*k]= r3n;
        output[n-1- 2*k]= r3n;
        output[n2 + 2*k+1]= r4;
        output[n-2 - 2 * k] = r4;
    }
}
