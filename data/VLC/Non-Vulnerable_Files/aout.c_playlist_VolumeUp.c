 */
int playlist_VolumeUp (playlist_t *pl, int value, float *volp)
{
    int ret = -1;
    float stepSize = var_InheritFloat (pl, "volume-step") / (float)AOUT_VOLUME_DEFAULT;
    float delta = value * stepSize;
    audio_output_t *aout = playlist_GetAout (pl);
    if (aout != NULL)
    {
        float vol = aout_VolumeGet (aout);
        if (vol >= 0.)
        {
            vol += delta;
            if (vol < 0.)
                vol = 0.;
            if (vol > 2.)
                vol = 2.;
            vol = (roundf (vol / stepSize)) * stepSize;
            if (volp != NULL)
                *volp = vol;
            ret = aout_VolumeSet (aout, vol);
        }
        vlc_object_release (aout);
    }
    return ret;
}
