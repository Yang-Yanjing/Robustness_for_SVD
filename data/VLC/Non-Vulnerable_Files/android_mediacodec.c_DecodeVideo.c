}
static picture_t *DecodeVideo(decoder_t *p_dec, block_t **pp_block)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    picture_t *p_pic = NULL;
    JNIEnv *env = NULL;
    struct H264ConvertState convert_state = { 0, 0 };
    if (!pp_block || !*pp_block)
        return NULL;
    block_t *p_block = *pp_block;
    if (p_sys->error_state) {
        block_Release(p_block);
        if (!p_sys->error_event_sent) {
            /* Signal the error to the Java. */
            jni_EventHardwareAccelerationError();
            p_sys->error_event_sent = true;
        }
        return NULL;
    }
    jni_attach_thread(&env, THREAD_NAME);
    if (p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED)) {
        block_Release(p_block);
        timestamp_FifoEmpty(p_sys->timestamp_fifo);
        if (p_sys->decoded) {
            /* Invalidate all pictures that are currently in flight
             * since flushing make all previous indices returned by
             * MediaCodec invalid. */
            if (p_sys->direct_rendering)
                InvalidateAllPictures(p_dec);
            (*env)->CallVoidMethod(env, p_sys->codec, p_sys->flush);
            if ((*env)->ExceptionOccurred(env)) {
                msg_Warn(p_dec, "Exception occurred in MediaCodec.flush");
                (*env)->ExceptionClear(env);
                p_sys->error_state = true;
            }
        }
        p_sys->decoded = false;
        jni_detach_thread();
        return NULL;
    }
    /* Use the aspect ratio provided by the input (ie read from packetizer).
     * Don't check the current value of the aspect ratio in fmt_out, since we
     * want to allow changes in it to propagate. */
    if (p_dec->fmt_in.video.i_sar_num != 0 && p_dec->fmt_in.video.i_sar_den != 0) {
        p_dec->fmt_out.video.i_sar_num = p_dec->fmt_in.video.i_sar_num;
        p_dec->fmt_out.video.i_sar_den = p_dec->fmt_in.video.i_sar_den;
    }
    jlong timeout = 0;
    const int max_polling_attempts = 50;
    int attempts = 0;
    while (true) {
        int index = (*env)->CallIntMethod(env, p_sys->codec, p_sys->dequeue_input_buffer, (jlong) 0);
        if ((*env)->ExceptionOccurred(env)) {
            msg_Err(p_dec, "Exception occurred in MediaCodec.dequeueInputBuffer");
            (*env)->ExceptionClear(env);
            p_sys->error_state = true;
            break;
        }
        if (index < 0) {
            GetOutput(p_dec, env, &p_pic, timeout);
            if (p_sys->error_state)
                break;
            if (p_pic) {
                /* If we couldn't get an available input buffer but a
                 * decoded frame is available, we return the frame
                 * without assigning NULL to *pp_block. The next call
                 * to DecodeVideo will try to send the input packet again.
                 */
                jni_detach_thread();
                return p_pic;
            }
            timeout = 30 * 1000;
            ++attempts;
            /* With opaque DR the output buffers are released by the
               vout therefore we implement a timeout for polling in
               order to avoid being indefinitely stalled in this loop. */
            if (p_sys->direct_rendering && attempts == max_polling_attempts) {
                picture_t *invalid_picture = decoder_NewPicture(p_dec);
                if (invalid_picture) {
                    invalid_picture->date = VLC_TS_INVALID;
                    picture_sys_t *p_picsys = invalid_picture->p_sys;
                    p_picsys->pf_display_callback = NULL;
                    p_picsys->pf_unlock_callback = NULL;
                    p_picsys->p_dec = NULL;
                    p_picsys->i_index = -1;
                    p_picsys->b_valid = false;
                }
                else {
                    /* If we cannot return a picture we must free the
                       block since the decoder will proceed with the
                       next block. */
                    block_Release(p_block);
                    *pp_block = NULL;
                }
                jni_detach_thread();
                return invalid_picture;
            }
            continue;
        }
        jobject buf = (*env)->GetObjectArrayElement(env, p_sys->input_buffers, index);
        jsize size = (*env)->GetDirectBufferCapacity(env, buf);
        uint8_t *bufptr = (*env)->GetDirectBufferAddress(env, buf);
        if (size > p_block->i_buffer)
            size = p_block->i_buffer;
        memcpy(bufptr, p_block->p_buffer, size);
        convert_h264_to_annexb(bufptr, size, p_sys->nal_size, &convert_state);
        int64_t ts = p_block->i_pts;
        if (!ts && p_block->i_dts)
            ts = p_block->i_dts;
        timestamp_FifoPut(p_sys->timestamp_fifo, p_block->i_pts ? VLC_TS_INVALID : p_block->i_dts);
        (*env)->CallVoidMethod(env, p_sys->codec, p_sys->queue_input_buffer, index, 0, size, ts, 0);
        (*env)->DeleteLocalRef(env, buf);
        if ((*env)->ExceptionOccurred(env)) {
            msg_Err(p_dec, "Exception in MediaCodec.queueInputBuffer");
            (*env)->ExceptionClear(env);
            p_sys->error_state = true;
            break;
        }
        p_sys->decoded = true;
        break;
    }
    if (p_sys->error_state) {
        if (p_pic)
            decoder_DeletePicture(p_dec, p_pic);
        jni_detach_thread();
        return NULL;
    }
    if (!p_pic)
        GetOutput(p_dec, env, &p_pic, 0);
    jni_detach_thread();
    block_Release(p_block);
    *pp_block = NULL;
    return p_pic;
}
