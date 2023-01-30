static int mode1314(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    const int d1 = FFABS(a1 - a8);
    const int d2 = FFABS(a2 - a7);
    const int d3 = FFABS(a3 - a6);
    const int mindiff = FFMIN(FFMIN(d1, d2), d3);
    if (mindiff == d2) {
        return (a2 + a7 + 1) >> 1;
    }
    if (mindiff == d3) {
        return (a3 + a6 + 1) >> 1;
    }
    return (a1 + a8 + 1) >> 1;
}
