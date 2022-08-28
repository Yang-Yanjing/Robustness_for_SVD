static AVDictionary *map_to_opts(RTSPState *rt)
{
    AVDictionary *opts = NULL;
    char buf[256];
    snprintf(buf, sizeof(buf), "%d", rt->buffer_size);
    av_dict_set(&opts, "buffer_size", buf, 0);
    return opts;
}
