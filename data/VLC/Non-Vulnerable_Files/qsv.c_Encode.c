 */
static block_t *Encode(encoder_t *this, picture_t *pic)
{
    encoder_t     *enc = (encoder_t *)this;
    encoder_sys_t *sys = enc->p_sys;
    async_task_t  *task = NULL;
    block_t       *block = NULL;
    mfxFrameSurface1 *frame = NULL;
    mfxStatus        sts;
    if (pic) {
        /* To avoid qsv -> vlc timestamp conversion overflow, we use timestamp relative
           to the first picture received. That way, vlc will overflow before us.
           (Thanks to funman for the idea) */
        if (!sys->offset_pts) // First frame
            sys->offset_pts = pic->date;
        pic->date -= sys->offset_pts;
        frame = qsv_frame_pool_Get(&sys->frames, pic);
        if (!frame) {
            msg_Warn(enc, "Unable to find an unlocked surface in the pool");
            return NULL;
        }
        /* Finds an available SyncPoint */
        for (size_t i = 0; i < sys->async_depth; i++)
            if ((sys->tasks + (i + sys->first_task) % sys->async_depth)->syncp == 0) {
                task = sys->tasks + (i + sys->first_task) % sys->async_depth;
                break;
            }
    } else
        /* If !pic, we are emptying encoder and tasks, so we force the SyncOperation */
        msg_Dbg(enc, "Emptying encoder");
    /* There is no available task, we need to synchronize */
    if (!task) {
        task = sys->tasks + sys->first_task;
        /* Synchronize and fill block_t. If the SyncOperation fails we leak :-/ (or we can segfault, ur choice) */
        if (MFXVideoCORE_SyncOperation(sys->session, task->syncp, QSV_SYNCPOINT_WAIT) == MFX_ERR_NONE) {
            block = task->block;
            block->i_buffer = task->bs.DataLength;
            block->p_buffer += task->bs.DataOffset;
            qsv_set_block_ts(enc, sys, block, &task->bs);
            qsv_set_block_flags(block, task->bs.FrameType);
            /* msg_Dbg(enc, "block->i_pts = %lld, block->i_dts = %lld", block->i_pts, block->i_dts); */
            /* msg_Dbg(enc, "FrameType = %#.4x, TimeStamp (pts) = %lld, DecodeTimeStamp = %lld", */
            /*         task->bs.FrameType, task->bs.TimeStamp, task->bs.DecodeTimeStamp); */
            /* Copied from x264.c: This isn't really valid for streams with B-frames */
            block->i_length = CLOCK_FREQ *
                enc->fmt_in.video.i_frame_rate_base /
                enc->fmt_in.video.i_frame_rate;
            // Buggy DTS (value comes from experiments)
            if (task->bs.DecodeTimeStamp < -10000)
                block->i_dts = sys->last_dts + block->i_length;
            sys->last_dts = block->i_dts;
        } else // Only happens on buggy drivers
            msg_Err(enc, "SyncOperation failed, outputting garbage data. "
                    "Updating your drivers and/or changing the encoding settings might resolve this");
        /* Reset the task now it has been synchronized and advances first_task pointer */
        task->syncp = 0;
        sys->first_task = (sys->first_task + 1) % sys->async_depth;
    }
    /* Allocate block_t and prepare mfxBitstream for encoder */
    if (!(task->block = block_Alloc(sys->params.mfx.BufferSizeInKB * 1000))) {
        msg_Err(enc, "Unable to allocate block for encoder output");
        return NULL;
    }
    memset(&task->bs, 0, sizeof(task->bs));
    task->bs.MaxLength = sys->params.mfx.BufferSizeInKB * 1000;
    task->bs.Data = task->block->p_buffer;
    for (;;) {
        sts = MFXVideoENCODE_EncodeFrameAsync(sys->session, 0, frame, &task->bs, &task->syncp);
        if (sts != MFX_WRN_DEVICE_BUSY)
            break;
        if (sys->busy_warn_counter++ % 16 == 0)
            msg_Dbg(enc, "Device is busy, let's wait and retry");
        msleep(QSV_BUSYWAIT_TIME);
    }
    // msg_Dbg(enc, "Encode async status: %d, Syncpoint = %tx", sts, (ptrdiff_t)task->syncp);
    if (sts == MFX_ERR_MORE_DATA)
        if (pic)
            msg_Dbg(enc, "Encoder feeding phase, more data is needed.");
        else
            msg_Dbg(enc, "Encoder is empty");
    else if (sts < MFX_ERR_NONE) {
        msg_Err(enc, "Encoder not ready or error (%d), trying a reset...", sts);
        MFXVideoENCODE_Reset(sys->session, &sys->params);
    }
    return block;
}
