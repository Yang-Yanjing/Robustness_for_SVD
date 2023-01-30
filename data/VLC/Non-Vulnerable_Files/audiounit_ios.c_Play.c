#pragma mark actual playback
static void Play (audio_output_t * p_aout, block_t * p_block)
{
    struct aout_sys_t *p_sys = p_aout->sys;
    if (p_block->i_nb_samples > 0) {
        /* move data to buffer */
        if (unlikely(!TPCircularBufferProduceBytes(&p_sys->circular_buffer, p_block->p_buffer, p_block->i_buffer)))
            msg_Warn(p_aout, "Audio buffer was dropped");
        if (!p_sys->i_bytes_per_sample)
            p_sys->i_bytes_per_sample = p_block->i_buffer / p_block->i_nb_samples;
    }
    block_Release(p_block);
}
