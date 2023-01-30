}
void playlist_EnableAudioFilter (playlist_t *pl, const char *name, bool add)
{
    audio_output_t *aout = playlist_GetAout (pl);
    aout_ChangeFilterString (VLC_OBJECT(pl), VLC_OBJECT(aout),
                             "audio-filter", name, add);
    if (aout != NULL)
        vlc_object_release (aout);
}
