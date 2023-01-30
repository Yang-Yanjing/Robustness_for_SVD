}
static void *DecodeAsync(decoder_t *p_dec, block_t **pp_block)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    HRESULT hr;
    if (!pp_block || !*pp_block)
        return NULL;
    block_t *p_block = *pp_block;
    if (p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY | BLOCK_FLAG_CORRUPTED))
    {
        block_Release(p_block);
        return NULL;
    }
    /* Dequeue all pending media events. */
    while ((hr = DequeueMediaEvent(p_dec)) == S_OK)
        continue;
    if (hr != MF_E_NO_EVENTS_AVAILABLE && FAILED(hr))
        goto error;
    /* Drain the output stream of the MFT before sending the input packet. */
    if (p_sys->pending_output_events > 0)
    {
        p_sys->pending_output_events -= 1;
        void *result = NULL;
        if (ProcessOutputStream(p_dec, p_sys->output_stream_id, &result))
            goto error;
        return result;
    }
    /* Poll the MFT and return decoded frames until the input stream is ready. */
    while (p_sys->pending_input_events == 0)
    {
        hr = DequeueMediaEvent(p_dec);
        if (hr == MF_E_NO_EVENTS_AVAILABLE)
        {
            /* Sleep for 1 ms to avoid excessive polling. */
            Sleep(1);
            continue;
        }
        if (FAILED(hr))
            goto error;
        if (p_sys->pending_output_events > 0)
        {
            p_sys->pending_output_events -= 1;
            void *result = NULL;
            if (ProcessOutputStream(p_dec, p_sys->output_stream_id, &result))
                goto error;
            return result;
        }
    }
    p_sys->pending_input_events -= 1;
    if (ProcessInputStream(p_dec, p_sys->input_stream_id, p_block))
        goto error;
    block_Release(p_block);
    *pp_block = NULL;
    return NULL;
error:
    msg_Err(p_dec, "Error in DecodeAsync()");
    block_Release(p_block);
    return NULL;
}
