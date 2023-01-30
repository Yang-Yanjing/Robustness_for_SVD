static int opt_add_vfilter(void *optctx, const char *opt, const char *arg)
{
    GROW_ARRAY(vfilters_list, nb_vfilters);
    vfilters_list[nb_vfilters - 1] = arg;
    return 0;
}
