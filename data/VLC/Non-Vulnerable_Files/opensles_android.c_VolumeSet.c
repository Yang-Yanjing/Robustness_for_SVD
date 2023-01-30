}
static int VolumeSet(audio_output_t *aout, float vol)
{
    if (!aout->sys->volumeItf)
        return -1;
    /* Convert UI volume to linear factor (cube) */
    vol = vol * vol * vol;
    /* millibels from linear amplification */
    int mb = lroundf(2000.f * log10f(vol));
    if (mb < SL_MILLIBEL_MIN)
        mb = SL_MILLIBEL_MIN;
    else if (mb > 0)
        mb = 0; /* maximum supported level could be higher: GetMaxVolumeLevel */
    SLresult r = SetVolumeLevel(aout->sys->volumeItf, mb);
    return (r == SL_RESULT_SUCCESS) ? 0 : -1;
}
