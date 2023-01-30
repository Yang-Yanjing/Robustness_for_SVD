static void copy_default_source_addrs(struct RTSPSource **addrs, int count,
                                      struct RTSPSource ***dest, int *dest_count)
{
    RTSPSource *rtsp_src, *rtsp_src2;
    int i;
    for (i = 0; i < count; i++) {
        rtsp_src = addrs[i];
        rtsp_src2 = av_malloc(sizeof(*rtsp_src2));
        if (!rtsp_src2)
            continue;
        memcpy(rtsp_src2, rtsp_src, sizeof(*rtsp_src));
        dynarray_add(dest, dest_count, rtsp_src2);
    }
}
