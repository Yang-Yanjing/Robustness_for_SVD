 */
int ff_mdct_init(MDCTContext *s, int nbits, int inverse)
{
    int n, n4, i;
    memset(s, 0, sizeof(*s));
    n = 1 << nbits;            /* nbits ranges from 12 to 8 inclusive */
    s->nbits = nbits;
    s->n = n;
    n4 = n >> 2;
    s->tcos = tcosarray[12-nbits];
    s->tsin = tsinarray[12-nbits];
    for(i=0;i<n4;i++)
    {
        int32_t ip = itofix32(i) + 0x2000;
        ip = ip >> nbits;
        /*I can't remember why this works, but it seems
          to agree for ~24 bits, maybe more!*/
        s->tsin[i] = - fsincos(ip<<16, &(s->tcos[i]));
        s->tcos[i] *=-1;
    }
    (&s->fft)->nbits = nbits-2;
    (&s->fft)->inverse = inverse;
    return 0;
}
