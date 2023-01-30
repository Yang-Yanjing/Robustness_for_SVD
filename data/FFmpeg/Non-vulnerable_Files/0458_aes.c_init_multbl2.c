static void init_multbl2(uint32_t tbl[][256], const int c[4],
                         const uint8_t *log8, const uint8_t *alog8,
                         const uint8_t *sbox)
{
    int i;
    for (i = 0; i < 256; i++) {
        int x = sbox[i];
        if (x) {
            int k, l, m, n;
            x = log8[x];
            k = alog8[x + log8[c[0]]];
            l = alog8[x + log8[c[1]]];
            m = alog8[x + log8[c[2]]];
            n = alog8[x + log8[c[3]]];
            tbl[0][i] = AV_NE(MKBETAG(k,l,m,n), MKTAG(k,l,m,n));
#if !CONFIG_SMALL
            tbl[1][i] = ROT(tbl[0][i], 8);
            tbl[2][i] = ROT(tbl[0][i], 16);
            tbl[3][i] = ROT(tbl[0][i], 24);
#endif
        }
    }
}
