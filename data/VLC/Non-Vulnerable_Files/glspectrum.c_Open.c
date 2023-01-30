 */
static int Open(vlc_object_t * p_this)
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    p_sys = p_filter->p_sys = (filter_sys_t*)malloc(sizeof(*p_sys));
    if (p_sys == NULL)
        return VLC_ENOMEM;
    /* Create the object for the thread */
    vlc_sem_init(&p_sys->ready, 0);
    p_sys->b_error = false;
    p_sys->i_width = var_InheritInteger(p_filter, "glspectrum-width");
    p_sys->i_height = var_InheritInteger(p_filter, "glspectrum-height");
    p_sys->i_channels = aout_FormatNbChannels(&p_filter->fmt_in.audio);
    p_sys->i_prev_nb_samples = 0;
    p_sys->p_prev_s16_buff = NULL;
    p_sys->f_rotationAngle = 0;
    p_sys->f_rotationIncrement = ROTATION_INCREMENT;
    /* Fetch the FFT window parameters */
    window_get_param( VLC_OBJECT( p_filter ), &p_sys->wind_param );
    /* Create the FIFO for the audio data. */
    p_sys->fifo = block_FifoNew();
    if (p_sys->fifo == NULL)
        goto error;
    /* Create the thread */
    if (vlc_clone(&p_sys->thread, Thread, p_filter,
                  VLC_THREAD_PRIORITY_VIDEO))
        goto error;
    /* Wait for the displaying thread to be ready. */
    vlc_sem_wait(&p_sys->ready);
    if (p_sys->b_error)
    {
        vlc_join(p_sys->thread, NULL);
        goto error;
    }
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
error:
    vlc_sem_destroy(&p_sys->ready);
    free(p_sys);
    return VLC_EGENERIC;
}
