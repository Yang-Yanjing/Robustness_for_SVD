}
static int Open (vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    void *opaque = var_InheritAddress (obj, "amem-data");
    sys->setup = var_InheritAddress (obj, "amem-setup");
    if (sys->setup != NULL)
    {
        sys->cleanup = var_InheritAddress (obj, "amem-cleanup");
        sys->setup_opaque = opaque;
    }
    else
    {
        sys->cleanup = NULL;
        sys->opaque = opaque;
        sys->rate = var_InheritInteger (obj, "amem-rate");
        sys->channels = var_InheritInteger (obj, "amem-channels");
    }
    sys->play = var_InheritAddress (obj, "amem-play");
    sys->pause = var_InheritAddress (obj, "amem-pause");
    sys->resume = var_InheritAddress (obj, "amem-resume");
    sys->flush = var_InheritAddress (obj, "amem-flush");
    sys->drain = var_InheritAddress (obj, "amem-drain");
    sys->set_volume = var_InheritAddress (obj, "amem-set-volume");
    sys->volume = 1.;
    sys->mute = false;
    sys->ready = false;
    if (sys->play == NULL)
    {
        free (sys);
        return VLC_EGENERIC;
    }
    aout->sys = sys;
    aout->start = Start;
    aout->stop = Stop;
    aout->time_get = NULL;
    aout->play = Play;
    aout->pause = Pause;
    aout->flush = Flush;
    if (sys->set_volume != NULL)
    {
        aout->volume_set = VolumeSet;
        aout->mute_set = MuteSet;
    }
    else
    {
        aout->volume_set = SoftVolumeSet;
        aout->mute_set = SoftMuteSet;
    }
    return VLC_SUCCESS;
}
