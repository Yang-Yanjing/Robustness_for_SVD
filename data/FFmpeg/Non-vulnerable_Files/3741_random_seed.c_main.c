int main(void)
{
    int i, j, retry;
    uint32_t seeds[N];
    for (retry=0; retry<3; retry++){
        for (i=0; i<N; i++){
            seeds[i] = av_get_random_seed();
            for (j=0; j<i; j++)
                if (seeds[j] == seeds[i])
                    goto retry;
        }
        printf("seeds OK\n");
        return 0;
        retry:;
    }
    printf("FAIL at %d with %X\n", j, seeds[j]);
    return 1;
}
