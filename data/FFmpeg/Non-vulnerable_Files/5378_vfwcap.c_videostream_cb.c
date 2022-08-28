static LRESULT CALLBACK videostream_cb(HWND hwnd, LPVIDEOHDR vdhdr)
{
    AVFormatContext *s;
    struct vfw_ctx *ctx;
    AVPacketList **ppktl, *pktl_next;
    s = (AVFormatContext *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
    ctx = s->priv_data;
    dump_videohdr(s, vdhdr);
    if(shall_we_drop(s))
        return FALSE;
    WaitForSingleObject(ctx->mutex, INFINITE);
    pktl_next = av_mallocz(sizeof(AVPacketList));
    if(!pktl_next)
        goto fail;
    if(av_new_packet(&pktl_next->pkt, vdhdr->dwBytesUsed) < 0) {
        av_free(pktl_next);
        goto fail;
    }
    pktl_next->pkt.pts = vdhdr->dwTimeCaptured;
    memcpy(pktl_next->pkt.data, vdhdr->lpData, vdhdr->dwBytesUsed);
    for(ppktl = &ctx->pktl ; *ppktl ; ppktl = &(*ppktl)->next);
    *ppktl = pktl_next;
    ctx->curbufsize += vdhdr->dwBytesUsed;
    SetEvent(ctx->event);
    ReleaseMutex(ctx->mutex);
    return TRUE;
fail:
    ReleaseMutex(ctx->mutex);
    return FALSE;
}
