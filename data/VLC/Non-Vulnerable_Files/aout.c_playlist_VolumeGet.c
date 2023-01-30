}
float playlist_VolumeGet (playlist_t *pl)
{
    float volume = -1.f;
    audio_output_t *aout = playlist_GetAout (pl);
    if (aout != NULL)
    {
        volume = aout_VolumeGet (aout);
        vlc_object_release (aout);
    }
    return volume;
}
