static int default_execute(AVFilterContext *ctx, avfilter_action_func *func, void *arg,
                           int *ret, int nb_jobs)
{
    int i;
    for (i = 0; i < nb_jobs; i++) {
        int r = func(ctx, arg, i, nb_jobs);
        if (ret)
            ret[i] = r;
    }
    return 0;
}
