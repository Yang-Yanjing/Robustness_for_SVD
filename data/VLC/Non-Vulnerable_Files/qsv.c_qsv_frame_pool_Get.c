 */
static mfxFrameSurface1 *qsv_frame_pool_Get(qsv_frame_pool_t *pool,
                                            picture_t *pic)
{
    for (size_t i = 0; i < pool->size; i++) {
        mfxFrameSurface1 *frame = &pool->frames[i];
        if (frame->Data.Locked)
            continue;
        if (frame->Data.MemId)
            picture_Release((picture_t *)frame->Data.MemId);
        frame->Data.MemId     = pic;
        frame->Data.Y         = pic->p[0].p_pixels;
        frame->Data.U         = pic->p[1].p_pixels;
        frame->Data.V         = pic->p[1].p_pixels + 1;
        frame->Data.TimeStamp = qsv_mtime_to_timestamp(pic->date);
        // Specify picture structure at runtime.
        if (pic->b_progressive)
            frame->Info.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
        else if (pic->b_top_field_first)
            frame->Info.PicStruct = MFX_PICSTRUCT_FIELD_TFF;
        else
            frame->Info.PicStruct = MFX_PICSTRUCT_FIELD_BFF;
        picture_Hold(pic);
        return frame;
    }
    return NULL;
}
