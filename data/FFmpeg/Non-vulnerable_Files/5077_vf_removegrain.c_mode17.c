static int mode17(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    REMOVE_GRAIN_SORT_AXIS
    const int l = FFMAX(FFMAX(mi1, mi2), FFMAX(mi3, mi4));
    const int u = FFMIN(FFMIN(ma1, ma2), FFMIN(ma3, ma4));
    return av_clip(c, FFMIN(l, u), FFMAX(l, u));
}
