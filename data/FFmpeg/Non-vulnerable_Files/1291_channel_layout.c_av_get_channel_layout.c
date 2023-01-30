uint64_t av_get_channel_layout(const char *name)
{
    return ff_get_channel_layout(name, 1);
}
