static int mode18(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    const int d1 = FFMAX(FFABS(c - a1), FFABS(c - a8));
    const int d2 = FFMAX(FFABS(c - a2), FFABS(c - a7));
    const int d3 = FFMAX(FFABS(c - a3), FFABS(c - a6));
    const int d4 = FFMAX(FFABS(c - a4), FFABS(c - a5));
    const int mindiff = FFMIN(FFMIN(d1, d2), FFMIN(d3, d4));
    if (mindiff == d4) {
        return av_clip(c, FFMIN(a4, a5), FFMAX(a4, a5));
    }
    if (mindiff == d2) {
        return av_clip(c, FFMIN(a2, a7), FFMAX(a2, a7));
    }
    if (mindiff == d3) {
        return av_clip(c, FFMIN(a3, a6), FFMAX(a3, a6));
    }
    return av_clip(c, FFMIN(a1, a8), FFMAX(a1, a8));
}
