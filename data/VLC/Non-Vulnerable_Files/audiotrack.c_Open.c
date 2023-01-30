}
static int Open(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->libmedia = InitLibrary(sys);
    if (sys->libmedia == NULL) {
        msg_Err(aout, "Could not initialize libmedia.so!");
        free(sys);
        return VLC_EGENERIC;
    }
    aout->sys = sys;
    aout->start = Start;
    aout->stop = Stop;
    aout_SoftVolumeInit(aout);
    return VLC_SUCCESS;
}
