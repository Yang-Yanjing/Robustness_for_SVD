 */
static int qsv_frame_pool_Init(qsv_frame_pool_t *pool,
                               mfxFrameAllocRequest *request,
                               uint64_t async_depth)
{
    size_t size = request->NumFrameSuggested + async_depth;
    pool->frames = calloc(size, sizeof(mfxFrameSurface1));
    if (unlikely(!pool->frames))
        return VLC_ENOMEM;
    pool->size = size;
    memcpy(&pool->fmt, &request->Info, sizeof(request->Info));
    for (size_t i = 0; i < size; i++) {
        memcpy(&pool->frames[i].Info, &request->Info, sizeof(request->Info));
        pool->frames[i].Data.Pitch = QSV_ALIGN(32, request->Info.Width);
    }
    return VLC_SUCCESS;
}
