static int get_buffer(AVCodecContext *avctx, AVFrame *frame, int flags)
{
    DecodeContext *decode = avctx->opaque;
    mfxFrameSurface1 *surf;
    AVBufferRef *surf_buf;
    int idx;
    for (idx = 0; idx < decode->nb_surfaces; idx++) {
        if (!decode->surface_used[idx])
            break;
    }
    if (idx == decode->nb_surfaces) {
        fprintf(stderr, "No free surfaces\n");
        return AVERROR(ENOMEM);
    }
    surf = av_mallocz(sizeof(*surf));
    if (!surf)
        return AVERROR(ENOMEM);
    surf_buf = av_buffer_create((uint8_t*)surf, sizeof(*surf), free_buffer,
                                &decode->surface_used[idx], AV_BUFFER_FLAG_READONLY);
    if (!surf_buf) {
        av_freep(&surf);
        return AVERROR(ENOMEM);
    }
    surf->Info       = decode->frame_info;
    surf->Data.MemId = &decode->surfaces[idx];
    frame->buf[0]  = surf_buf;
    frame->data[3] = (uint8_t*)surf;
    decode->surface_used[idx] = 1;
    return 0;
}
