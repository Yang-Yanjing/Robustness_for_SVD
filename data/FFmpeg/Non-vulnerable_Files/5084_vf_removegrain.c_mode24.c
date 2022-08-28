static int mode24(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    REMOVE_GRAIN_SORT_AXIS
    const int linediff1 = ma1 - mi1;
    const int linediff2 = ma2 - mi2;
    const int linediff3 = ma3 - mi3;
    const int linediff4 = ma4 - mi4;
    const int tu1 = c - ma1;
    const int tu2 = c - ma2;
    const int tu3 = c - ma3;
    const int tu4 = c - ma4;
    const int u1 = FFMIN(tu1, linediff1 - tu1);
    const int u2 = FFMIN(tu2, linediff2 - tu2);
    const int u3 = FFMIN(tu3, linediff3 - tu3);
    const int u4 = FFMIN(tu4, linediff4 - tu4);
    const int u = FFMAX(FFMAX(FFMAX(u1, u2), FFMAX(u3, u4)), 0);
    const int td1 = mi1 - c;
    const int td2 = mi2 - c;
    const int td3 = mi3 - c;
    const int td4 = mi4 - c;
    const int d1 = FFMIN(td1, linediff1 - td1);
    const int d2 = FFMIN(td2, linediff2 - td2);
    const int d3 = FFMIN(td3, linediff3 - td3);
    const int d4 = FFMIN(td4, linediff4 - td4);
    const int d = FFMAX(FFMAX(FFMAX(d1, d2), FFMAX(d3, d4)), 0);
    return c - u + d;  
}
