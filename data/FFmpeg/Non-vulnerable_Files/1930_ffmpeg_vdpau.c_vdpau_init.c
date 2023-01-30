int vdpau_init(AVCodecContext *s)
{
    InputStream *ist = s->opaque;
    if (!ist->hwaccel_ctx) {
        int ret = vdpau_alloc(s);
        if (ret < 0)
            return ret;
    }
    ist->hwaccel_get_buffer    = vdpau_get_buffer;
    ist->hwaccel_retrieve_data = vdpau_retrieve_data;
    return 0;
}
