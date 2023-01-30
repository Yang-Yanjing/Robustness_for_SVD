static void compat_unref_buffer(void *opaque, uint8_t *data)
{
    AVBufferRef *buf = opaque;
    AV_NOWARN_DEPRECATED(
    av_buffer_unref(&buf);
    )
}
