}
static int WriteBuffer(audio_output_t *aout)
{
    aout_sys_t *sys = aout->sys;
    const size_t unit_size = sys->samples_per_buf * bytesPerSample();
    block_t *b = sys->p_buffer_chain;
    if (!b)
        return false;
    /* Check if we can fill at least one buffer unit by chaining blocks */
    if (b->i_buffer < unit_size) {
        if (!b->p_next)
            return false;
        ssize_t needed = unit_size - b->i_buffer;
        for (block_t *next = b->p_next; next; next = next->p_next) {
            needed -= next->i_buffer;
            if (needed <= 0)
                break;
        }
        if (needed > 0)
            return false;
    }
    SLAndroidSimpleBufferQueueState st;
    SLresult res = GetState(sys->playerBufferQueue, &st);
    if (unlikely(res != SL_RESULT_SUCCESS)) {
        msg_Err(aout, "Could not query buffer queue state in %s (%lu)", __func__, res);
        return false;
    }
    if (st.count == OPENSLES_BUFFERS)
        return false;
    size_t done = 0;
    while (done < unit_size) {
        size_t cur = b->i_buffer;
        if (cur > unit_size - done)
            cur = unit_size - done;
        memcpy(&sys->buf[unit_size * sys->next_buf + done], b->p_buffer, cur);
        b->i_buffer -= cur;
        b->p_buffer += cur;
        done += cur;
        block_t *next = b->p_next;
        if (b->i_buffer == 0) {
            block_Release(b);
            b = NULL;
        }
        if (done == unit_size)
            break;
        else
            b = next;
    }
    sys->p_buffer_chain = b;
    if (!b)
        sys->pp_buffer_last = &sys->p_buffer_chain;
    SLresult r = Enqueue(sys->playerBufferQueue,
        &sys->buf[unit_size * sys->next_buf], unit_size);
    sys->samples -= sys->samples_per_buf;
    if (r == SL_RESULT_SUCCESS) {
        if (++sys->next_buf == OPENSLES_BUFFERS)
            sys->next_buf = 0;
        return true;
    } else {
        /* XXX : if writing fails, we don't retry */
        msg_Err(aout, "error %lu when writing %d bytes %s",
                r, b->i_buffer,
                (r == SL_RESULT_BUFFER_INSUFFICIENT) ? " (buffer insufficient)" : "");
        return false;
    }
}
