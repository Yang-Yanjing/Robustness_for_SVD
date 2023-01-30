}
static void *DecodeSync(decoder_t *p_dec, block_t **pp_block)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if (!pp_block || !*pp_block)
        return NULL;
    block_t *p_block = *pp_block;
    if (p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY | BLOCK_FLAG_CORRUPTED))
    {
        block_Release(p_block);
        return NULL;
    }
    /* Drain the output stream before sending the input packet. */
    void *result = NULL;
    if (ProcessOutputStream(p_dec, p_sys->output_stream_id, &result))
        goto error;
    if (result)
        return result;
    if (ProcessInputStream(p_dec, p_sys->input_stream_id, p_block))
        goto error;
    block_Release(p_block);
    *pp_block = NULL;
    return NULL;
error:
    msg_Err(p_dec, "Error in DecodeSync()");
    if (p_block)
        block_Release(p_block);
    return NULL;
}
