}
vlm_message_t *vlm_MessageNew (const char *a, const char *fmt, ...)
{
    VLC_UNUSED (a);
    VLC_UNUSED (fmt);
    return vlm_MessageSimpleNew (a);
}
