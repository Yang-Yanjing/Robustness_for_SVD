static int mode23(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    REMOVE_GRAIN_SORT_AXIS
    const int linediff1 = ma1 - mi1;
    const int linediff2 = ma2 - mi2;
    const int linediff3 = ma3 - mi3;
    const int linediff4 = ma4 - mi4;
    const int u1 = FFMIN(c - ma1, linediff1);
    const int u2 = FFMIN(c - ma2, linediff2);
    const int u3 = FFMIN(c - ma3, linediff3);
    const int u4 = FFMIN(c - ma4, linediff4);
    const int u = FFMAX(FFMAX(FFMAX(u1, u2), FFMAX(u3, u4)), 0);
    const int d1 = FFMIN(mi1 - c, linediff1);
    const int d2 = FFMIN(mi2 - c, linediff2);
    const int d3 = FFMIN(mi3 - c, linediff3);
    const int d4 = FFMIN(mi4 - c, linediff4);
    const int d = FFMAX(FFMAX(FFMAX(d1, d2), FFMAX(d3, d4)), 0);
    return c - u + d;  
}
