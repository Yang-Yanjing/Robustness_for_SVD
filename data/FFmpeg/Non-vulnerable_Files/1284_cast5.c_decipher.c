static void decipher(AVCAST5* cs, uint8_t* dst, const uint8_t* src, uint8_t *iv)
{
    uint32_t f, I, r, l;
    l = AV_RB32(src);
    r = AV_RB32(src + 4);
    if (cs->rounds == 16) {
        F1(l, r, 16);
        F3(r, l, 15);
        F2(l, r, 14);
        F1(r, l, 13);
    }
    F3(l, r, 12);
    F2(r, l, 11);
    F1(l, r, 10);
    F3(r, l, 9);
    F2(l, r, 8);
    F1(r, l, 7);
    F3(l, r, 6);
    F2(r, l, 5);
    F1(l, r, 4);
    F3(r, l, 3);
    F2(l, r, 2);
    F1(r, l, 1);
    if (iv) {
        r ^= AV_RB32(iv);
        l ^= AV_RB32(iv + 4);
        memcpy(iv, src, 8);
    }
    AV_WB32(dst, r);
    AV_WB32(dst + 4, l);
}
