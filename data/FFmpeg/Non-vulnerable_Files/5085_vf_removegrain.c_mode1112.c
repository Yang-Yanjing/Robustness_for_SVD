static int mode1112(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    const int sum = 4 * c + 2 * (a2 + a4 + a5 + a7) + a1 + a3 + a6 + a8;
    const int val = (sum + 8) >> 4;
    return val;
}
