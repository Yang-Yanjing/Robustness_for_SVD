}
static int VolumeSet(audio_output_t * p_aout, float volume)
{
    struct aout_sys_t *p_sys = p_aout->sys;
    OSStatus ostatus;
    aout_VolumeReport(p_aout, volume);
    p_sys->f_volume = volume;
    /* Set volume for output unit */
    ostatus = AudioQueueSetParameter(p_sys->audioQueueRef, kAudioQueueParam_Volume, volume * volume * volume);
    return ostatus;
}
