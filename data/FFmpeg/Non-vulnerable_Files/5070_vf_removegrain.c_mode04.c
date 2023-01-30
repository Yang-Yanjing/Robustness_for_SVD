static int mode04(int c, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    int a[8] = { a1, a2, a3, a4, a5, a6, a7, a8 };
    qsort(&a, 8, sizeof(a[0]), cmp_int);
    return av_clip(c, a[4 - 1 ], a[5 - 1]);
}
