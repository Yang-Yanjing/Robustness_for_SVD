static int softthresh_c(PP7Context *p, int16_t *src, int qp)
{
    int i;
    int a;
    a = src[0] * factor[0];
    for (i = 1; i < 16; i++) {
        unsigned int threshold1 = p->thres2[qp][i];
        unsigned int threshold2 = threshold1 << 1;
        int level = src[i];
        if (((unsigned)(level + threshold1)) > threshold2) {
            if (level > 0)
                a += (level - (int)threshold1) * factor[i];
            else
                a += (level + (int)threshold1) * factor[i];
        }
    }
    return (a + (1 << 11)) >> 12;
}
