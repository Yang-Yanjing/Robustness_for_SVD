static void *alloc_array_elem(void **array, size_t elsize,
                              int *size, int *max_size)
{
    void *ret;
    if (*size == *max_size) {
        int m = FFMAX(32, FFMIN(*max_size, INT_MAX / 2) * 2);
        if (*size >= m)
            return NULL;
        *array = av_realloc_f(*array, m, elsize);
        if (!*array)
            return NULL;
        *max_size = m;
    }
    ret = (char *)*array + elsize * *size;
    memset(ret, 0, elsize);
    (*size)++;
    return ret;
}
