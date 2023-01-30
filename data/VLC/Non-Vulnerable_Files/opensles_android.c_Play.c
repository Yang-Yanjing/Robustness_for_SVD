 *****************************************************************************/
static void Play(audio_output_t *aout, block_t *p_buffer)
{
    aout_sys_t *sys = aout->sys;
    p_buffer->p_next = NULL; /* Make sur our linked list doesn't use old references */
    vlc_mutex_lock(&sys->lock);
    sys->samples += p_buffer->i_buffer / bytesPerSample();
    /* Hold this block until we can write it into the OpenSL buffer */
    block_ChainLastAppend(&sys->pp_buffer_last, p_buffer);
    /* Fill OpenSL buffer */
    while (WriteBuffer(aout))
        ;
    vlc_mutex_unlock(&sys->lock);
}
