static void append_source_addrs(char *buf, int size, const char *name,
                                int count, struct RTSPSource **addrs)
{
    int i;
    if (!count)
        return;
    av_strlcatf(buf, size, "&%s=%s", name, addrs[0]->addr);
    for (i = 1; i < count; i++)
        av_strlcatf(buf, size, ",%s", addrs[i]->addr);
}
