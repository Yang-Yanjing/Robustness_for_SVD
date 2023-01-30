static void encipher(AVCAST5* cs, uint8_t* dst, const uint8_t* src)
{
    uint32_t r, l, f, I;
    l = AV_RB32(src);
    r = AV_RB32(src + 4);
    F1(l, r, 1);
    F2(r, l, 2);
    F3(l, r, 3);
    F1(r, l, 4);
    F2(l, r, 5);
    F3(r, l, 6);
    F1(l, r, 7);
    F2(r, l, 8);
    F3(l, r, 9);
    F1(r, l, 10);
    F2(l, r, 11);
    F3(r, l, 12);
    if (cs->rounds == 16) {
        F1(l, r, 13);
        F2(r, l, 14);
        F3(l, r, 15);
        F1(r, l, 16);
    }
    AV_WB32(dst, r);
    AV_WB32(dst + 4, l);
}
