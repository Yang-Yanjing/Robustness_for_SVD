}
int playlist_MuteGet (playlist_t *pl)
{
    int mute = -1;
    audio_output_t *aout = playlist_GetAout (pl);
    if (aout != NULL)
    {
        mute = aout_MuteGet (aout);
        vlc_object_release (aout);
    }
    return mute;
}
