static void free_tracked_methods(RTMPContext *rt)
{
    int i;
    for (i = 0; i < rt->nb_tracked_methods; i ++)
        av_freep(&rt->tracked_methods[i].name);
    av_freep(&rt->tracked_methods);
    rt->tracked_methods_size = 0;
    rt->nb_tracked_methods   = 0;
}
