static int vfw_read_close(AVFormatContext *s)
{
    struct vfw_ctx *ctx = s->priv_data;
    AVPacketList *pktl;
    if(ctx->hwnd) {
        SendMessage(ctx->hwnd, WM_CAP_SET_CALLBACK_VIDEOSTREAM, 0, 0);
        SendMessage(ctx->hwnd, WM_CAP_DRIVER_DISCONNECT, 0, 0);
        DestroyWindow(ctx->hwnd);
    }
    if(ctx->mutex)
        CloseHandle(ctx->mutex);
    if(ctx->event)
        CloseHandle(ctx->event);
    pktl = ctx->pktl;
    while (pktl) {
        AVPacketList *next = pktl->next;
        av_free_packet(&pktl->pkt);
        av_free(pktl);
        pktl = next;
    }
    return 0;
}
