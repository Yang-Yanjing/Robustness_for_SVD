static int mode19(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    const int sum = a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8;
    const int val = (sum + 4) >> 3;
    return val;
}
