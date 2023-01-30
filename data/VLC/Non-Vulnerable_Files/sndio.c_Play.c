}
static void Play (audio_output_t *aout, block_t *block)
{
    aout_sys_t *sys = aout->sys;
    sio_write (sys->hdl, block->p_buffer, block->i_buffer);
    sys->delay += block->i_nb_samples;
    block_Release (block);
}
