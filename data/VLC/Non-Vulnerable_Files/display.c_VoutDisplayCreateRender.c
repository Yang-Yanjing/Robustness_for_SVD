static void DummyVoutSendDisplayEventMouse(vout_thread_t *, vlc_mouse_t *fallback, const vlc_mouse_t *m);
static void VoutDisplayCreateRender(vout_display_t *vd)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    osys->filters = NULL;
    video_format_t v_src = vd->source;
    v_src.i_sar_num = 0;
    v_src.i_sar_den = 0;
    video_format_t v_dst = vd->fmt;
    v_dst.i_sar_num = 0;
    v_dst.i_sar_den = 0;
    video_format_t v_dst_cmp = v_dst;
    if ((v_src.i_chroma == VLC_CODEC_J420 && v_dst.i_chroma == VLC_CODEC_I420) ||
        (v_src.i_chroma == VLC_CODEC_J422 && v_dst.i_chroma == VLC_CODEC_I422) ||
        (v_src.i_chroma == VLC_CODEC_J440 && v_dst.i_chroma == VLC_CODEC_I440) ||
        (v_src.i_chroma == VLC_CODEC_J444 && v_dst.i_chroma == VLC_CODEC_I444))
        v_dst_cmp.i_chroma = v_src.i_chroma;
    const bool convert = memcmp(&v_src, &v_dst_cmp, sizeof(v_src)) != 0;
    if (!convert)
        return;
    msg_Dbg(vd, "A filter to adapt decoder to display is needed");
    osys->filters = filter_chain_New(vd, "video filter2", false,
                                     FilterAllocationInit,
                                     FilterAllocationClean, vd);
    assert(osys->filters); /* TODO critical */
    /* */
    es_format_t src;
    es_format_InitFromVideo(&src, &v_src);
    /* */
    filter_t *filter;
    for (int i = 0; i < 1 + (v_dst_cmp.i_chroma != v_dst.i_chroma); i++) {
        es_format_t dst;
        es_format_InitFromVideo(&dst, i == 0 ? &v_dst : &v_dst_cmp);
        filter_chain_Reset(osys->filters, &src, &dst);
        filter = filter_chain_AppendFilter(osys->filters,
                                           NULL, NULL, &src, &dst);
        es_format_Clean(&dst);
        if (filter)
            break;
    }
    es_format_Clean(&src);
    if (!filter)
        msg_Err(vd, "Failed to adapt decoder format to display");
}
