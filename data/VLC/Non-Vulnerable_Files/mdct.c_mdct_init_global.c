/* init MDCT */
int mdct_init_global(void)
{
    int i,j,m;
    /* although seemingly degenerate, these cannot actually be merged together without
       a substantial increase in error which is unjustified by the tiny memory savings*/
    tcosarray[0] = tcos0; tcosarray[1] = tcos1; tcosarray[2] = tcos2; tcosarray[3] = tcos3;tcosarray[4] = tcos4;
    tsinarray[0] = tsin0; tsinarray[1] = tsin1; tsinarray[2] = tsin2; tsinarray[3] = tsin3;tsinarray[4] = tsin4;
    /* init the MDCT bit reverse table here rather then in fft_init */
    for(i=0;i<1024;i++)           /*hard coded to a 2048 bit rotation*/
    {                             /*smaller sizes can reuse the largest*/
        m=0;
        for(j=0;j<10;j++)
        {
            m |= ((i >> j) & 1) << (10-j-1);
        }
       revtab0[i]=m;
    }
    fft_init_global();
    return 0;
}
