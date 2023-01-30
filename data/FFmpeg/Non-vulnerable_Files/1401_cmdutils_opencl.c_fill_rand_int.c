static void fill_rand_int(int *data, int n)
{
    int i;
    srand(av_gettime());
    for (i = 0; i < n; i++)
        data[i] = rand();
}
