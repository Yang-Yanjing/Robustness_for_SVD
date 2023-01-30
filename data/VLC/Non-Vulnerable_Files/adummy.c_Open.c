}
static int Open(vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout->start = Start;
    aout->time_get = NULL;
    aout->play = Play;
    aout->pause = NULL;
    aout->flush = Flush;
    aout->stop = NULL;
    aout->volume_set = NULL;
    aout->mute_set = NULL;
    return VLC_SUCCESS;
}
