static double clean_mean(double *values, int count)
{
    double mean = 0;
    int cut = count / 5;
    int x;
    qsort(values, count, sizeof(double), (void*)cmp);
    for (x = cut; x < count - cut; x++) {
        mean += values[x];
    }
    return mean / (count - cut * 2);
}
