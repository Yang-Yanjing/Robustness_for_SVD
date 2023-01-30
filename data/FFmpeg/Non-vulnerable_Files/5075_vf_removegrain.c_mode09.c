static int mode09(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    REMOVE_GRAIN_SORT_AXIS
    const int d1 = ma1 - mi1;
    const int d2 = ma2 - mi2;
    const int d3 = ma3 - mi3;
    const int d4 = ma4 - mi4;
    const int mindiff = FFMIN(FFMIN(d1, d2), FFMIN(d3, d4));
    if (mindiff == d4) {
        return av_clip(c, mi4, ma4);
    } else if (mindiff == d2) {
        return av_clip(c, mi2, ma2);
    } else if (mindiff == d3) {
        return av_clip(c, mi3, ma3);
    }
    return av_clip(c, mi1, ma1);
}
