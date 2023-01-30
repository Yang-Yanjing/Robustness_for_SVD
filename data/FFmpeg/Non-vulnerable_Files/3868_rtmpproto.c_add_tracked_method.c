static int handle_chunk_size(URLContext *s, RTMPPacket *pkt);
static int add_tracked_method(RTMPContext *rt, const char *name, int id)
{
    int err;
    if (rt->nb_tracked_methods + 1 > rt->tracked_methods_size) {
        rt->tracked_methods_size = (rt->nb_tracked_methods + 1) * 2;
        if ((err = av_reallocp(&rt->tracked_methods, rt->tracked_methods_size *
                               sizeof(*rt->tracked_methods))) < 0) {
            rt->nb_tracked_methods = 0;
            rt->tracked_methods_size = 0;
            return err;
        }
    }
    rt->tracked_methods[rt->nb_tracked_methods].name = av_strdup(name);
    if (!rt->tracked_methods[rt->nb_tracked_methods].name)
        return AVERROR(ENOMEM);
    rt->tracked_methods[rt->nb_tracked_methods].id = id;
    rt->nb_tracked_methods++;
    return 0;
}
