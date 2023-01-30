}
static int MuteSet(audio_output_t *aout, bool mute)
{
    if (!aout->sys->volumeItf)
        return -1;
    SLresult r = SetMute(aout->sys->volumeItf, mute);
    return (r == SL_RESULT_SUCCESS) ? 0 : -1;
}
