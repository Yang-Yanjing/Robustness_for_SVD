FF_DISABLE_DEPRECATION_WARNINGS
static void compat_free_buffer(void *opaque, uint8_t *data)
{
    AVFilterBufferRef *buf = opaque;
    AV_NOWARN_DEPRECATED(
    avfilter_unref_buffer(buf);
    )
}
