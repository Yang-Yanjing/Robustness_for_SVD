int main(void)
{
    volatile int val = 1;
    int res;
    res = avpriv_atomic_int_add_and_fetch(&val, 1);
    av_assert0(res == 2);
    avpriv_atomic_int_set(&val, 3);
    res = avpriv_atomic_int_get(&val);
    av_assert0(res == 3);
    return 0;
}
