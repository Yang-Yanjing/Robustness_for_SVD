#pragma mark initialization
static int Open(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    aout->sys = sys;
    aout->start = Start;
    aout->stop = Stop;
    aout->volume_set = VolumeSet;
    /* reset volume */
    aout_VolumeReport(aout, 1.0);
    return VLC_SUCCESS;
}
