}
static int modulecmp (const void *a, const void *b)
{
    const module_t *const *ma = a, *const *mb = b;
    /* Note that qsort() uses _ascending_ order,
     * so the smallest module is the one with the biggest score. */
    return (*mb)->i_score - (*ma)->i_score;
}
