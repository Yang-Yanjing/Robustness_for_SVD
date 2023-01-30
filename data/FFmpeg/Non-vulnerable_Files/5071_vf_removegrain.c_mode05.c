static int mode05(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    REMOVE_GRAIN_SORT_AXIS
    const int c1 = FFABS(c - av_clip(c, mi1, ma1));
    const int c2 = FFABS(c - av_clip(c, mi2, ma2));
    const int c3 = FFABS(c - av_clip(c, mi3, ma3));
    const int c4 = FFABS(c - av_clip(c, mi4, ma4));
    const int mindiff = FFMIN(FFMIN(c1, c2), FFMIN(c3, c4));
    
    if (mindiff == c4) {
        return av_clip(c, mi4, ma4);
    } else if (mindiff == c2) {
        return av_clip(c, mi2, ma2);
    } else if (mindiff == c3) {
        return av_clip(c, mi3, ma3);
    }
    return av_clip(c, mi1, ma1);
}
