#pragma mark initialization
static int Open(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    vlc_mutex_init(&sys->lock);
    vlc_cond_init(&sys->cond);
    sys->b_paused = false;
    aout->sys = sys;
    aout->start = Start;
    aout->stop = Stop;
    return VLC_SUCCESS;
}
