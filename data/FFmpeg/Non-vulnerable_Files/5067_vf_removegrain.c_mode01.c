    const int mi4 = FFMIN(a4, a5);
static int mode01(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    const int mi = FFMIN(FFMIN(FFMIN(a1, a2), FFMIN(a3, a4)), FFMIN(FFMIN(a5, a6), FFMIN(a7, a8)));
    const int ma = FFMAX(FFMAX(FFMAX(a1, a2), FFMAX(a3, a4)), FFMAX(FFMAX(a5, a6), FFMAX(a7, a8)));
    return av_clip(c, mi, ma);
}
