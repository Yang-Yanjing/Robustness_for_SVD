#pragma mark actual playback
static void Play(audio_output_t * p_aout, block_t * p_block)
{
    struct aout_sys_t *p_sys = p_aout->sys;
    if (p_block->i_nb_samples > 0) {
        /* Do the channel reordering */
        if (p_sys->chans_to_reorder && !p_sys->b_digital) {
           aout_ChannelReorder(p_block->p_buffer,
                               p_block->i_buffer,
                               p_sys->chans_to_reorder,
                               p_sys->chan_table,
                               VLC_CODEC_FL32);
        }
        /* move data to buffer */
        if (unlikely(!TPCircularBufferProduceBytes(&p_sys->circular_buffer, p_block->p_buffer, p_block->i_buffer)))
            msg_Warn(p_aout, "dropped buffer");
        if (!p_sys->i_bytes_per_sample)
            p_sys->i_bytes_per_sample = p_block->i_buffer / p_block->i_nb_samples;
    }
    block_Release(p_block);
}
