static void del_tracked_method(RTMPContext *rt, int index)
{
    memmove(&rt->tracked_methods[index], &rt->tracked_methods[index + 1],
            sizeof(*rt->tracked_methods) * (rt->nb_tracked_methods - index - 1));
    rt->nb_tracked_methods--;
}
