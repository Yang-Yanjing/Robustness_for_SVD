}
int playlist_VolumeSet (playlist_t *pl, float vol)
{
    int ret = -1;
    audio_output_t *aout = playlist_GetAout (pl);
    if (aout != NULL)
    {
        ret = aout_VolumeSet (aout, vol);
        vlc_object_release (aout);
    }
    return ret;
}
