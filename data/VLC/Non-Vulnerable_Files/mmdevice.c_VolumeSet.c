}
static int VolumeSet(audio_output_t *aout, float vol)
{
    aout_sys_t *sys = aout->sys;
    vol = vol * vol * vol; /* ISimpleAudioVolume is tapered linearly. */
    EnterCriticalSection(&sys->lock);
    sys->volume = vol;
    WakeConditionVariable(&sys->work);
    LeaveCriticalSection(&sys->lock);
    return 0;
}
