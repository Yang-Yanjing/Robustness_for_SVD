static int
dshow_read_close(AVFormatContext *s)
{
    struct dshow_ctx *ctx = s->priv_data;
    AVPacketList *pktl;
    if (ctx->control) {
        IMediaControl_Stop(ctx->control);
        IMediaControl_Release(ctx->control);
    }
    if (ctx->media_event)
        IMediaEvent_Release(ctx->media_event);
    if (ctx->graph) {
        IEnumFilters *fenum;
        int r;
        r = IGraphBuilder_EnumFilters(ctx->graph, &fenum);
        if (r == S_OK) {
            IBaseFilter *f;
            IEnumFilters_Reset(fenum);
            while (IEnumFilters_Next(fenum, 1, &f, NULL) == S_OK) {
                if (IGraphBuilder_RemoveFilter(ctx->graph, f) == S_OK)
                    IEnumFilters_Reset(fenum); 

                IBaseFilter_Release(f);
            }
            IEnumFilters_Release(fenum);
        }
        IGraphBuilder_Release(ctx->graph);
    }
    if (ctx->capture_pin[VideoDevice])
        libAVPin_Release(ctx->capture_pin[VideoDevice]);
    if (ctx->capture_pin[AudioDevice])
        libAVPin_Release(ctx->capture_pin[AudioDevice]);
    if (ctx->capture_filter[VideoDevice])
        libAVFilter_Release(ctx->capture_filter[VideoDevice]);
    if (ctx->capture_filter[AudioDevice])
        libAVFilter_Release(ctx->capture_filter[AudioDevice]);
    if (ctx->device_pin[VideoDevice])
        IPin_Release(ctx->device_pin[VideoDevice]);
    if (ctx->device_pin[AudioDevice])
        IPin_Release(ctx->device_pin[AudioDevice]);
    if (ctx->device_filter[VideoDevice])
        IBaseFilter_Release(ctx->device_filter[VideoDevice]);
    if (ctx->device_filter[AudioDevice])
        IBaseFilter_Release(ctx->device_filter[AudioDevice]);
    if (ctx->device_name[0])
        av_freep(&ctx->device_name[0]);
    if (ctx->device_name[1])
        av_freep(&ctx->device_name[1]);
    if(ctx->mutex)
        CloseHandle(ctx->mutex);
    if(ctx->event[0])
        CloseHandle(ctx->event[0]);
    if(ctx->event[1])
        CloseHandle(ctx->event[1]);
    pktl = ctx->pktl;
    while (pktl) {
        AVPacketList *next = pktl->next;
        av_free_packet(&pktl->pkt);
        av_free(pktl);
        pktl = next;
    }
    CoUninitialize();
    return 0;
}
