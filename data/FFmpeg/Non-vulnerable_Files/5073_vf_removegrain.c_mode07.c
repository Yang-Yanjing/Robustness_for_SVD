static int mode07(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    REMOVE_GRAIN_SORT_AXIS
    const int d1 = ma1 - mi1;
    const int d2 = ma2 - mi2;
    const int d3 = ma3 - mi3;
    const int d4 = ma4 - mi4;
    const int cli1 = av_clip(c, mi1, ma1);
    const int cli2 = av_clip(c, mi2, ma2);
    const int cli3 = av_clip(c, mi3, ma3);
    const int cli4 = av_clip(c, mi4, ma4);
    const int c1 = FFABS(c - cli1) + d1;
    const int c2 = FFABS(c - cli2) + d2;
    const int c3 = FFABS(c - cli3) + d3;
    const int c4 = FFABS(c - cli4) + d4;
    const int mindiff = FFMIN(FFMIN(c1, c2), FFMIN(c3, c4));
    if (mindiff == c4) {
        return cli4;
    } else if (mindiff == c2) {
        return cli2;
    } else if (mindiff == c3) {
        return cli3;
    }
    return cli1;
}
