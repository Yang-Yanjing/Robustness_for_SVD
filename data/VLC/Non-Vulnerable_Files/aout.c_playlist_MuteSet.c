}
int playlist_MuteSet (playlist_t *pl, bool mute)
{
    int ret = -1;
    audio_output_t *aout = playlist_GetAout (pl);
    if (aout != NULL)
    {
        ret = aout_MuteSet (aout, mute);
        vlc_object_release (aout);
    }
    return ret;
}
