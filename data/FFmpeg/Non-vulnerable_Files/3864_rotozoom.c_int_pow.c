static int64_t int_pow(int64_t a, int p)
{
    int64_t v = FIXP;
    for (; p; p--) {
        v *= a;
        v /= FIXP;
    }
    return v;
}
