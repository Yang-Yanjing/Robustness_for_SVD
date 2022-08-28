static void init_thres2(PP7Context *p)
{
    int qp, i;
    int bias = 0; 
    for (qp = 0; qp < 99; qp++) {
        for (i = 0; i < 16; i++) {
            p->thres2[qp][i] = ((i&1) ? SN2 : SN0) * ((i&4) ? SN2 : SN0) * FFMAX(1, qp) * (1<<2) - 1 - bias;
        }
    }
}
