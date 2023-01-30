};
static void Play (audio_output_t *aout, block_t *block)
{
    aout_sys_t *sys = aout->sys;
    sys->play (sys->opaque, block->p_buffer, block->i_nb_samples,
               block->i_pts);
    block_Release (block);
}
