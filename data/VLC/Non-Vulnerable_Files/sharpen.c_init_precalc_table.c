}
static void init_precalc_table(filter_sys_t *p_filter, float sigma)
{
    for(int i = 0; i < 512; ++i)
    {
        p_filter->tab_precalc[i] = (i - 256) * sigma;
    }
}
