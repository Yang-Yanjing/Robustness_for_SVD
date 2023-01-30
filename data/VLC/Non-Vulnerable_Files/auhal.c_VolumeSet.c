}
static int VolumeSet(audio_output_t * p_aout, float volume)
{
    struct aout_sys_t *p_sys = p_aout->sys;
    OSStatus ostatus = 0;
    if(p_sys->b_digital)
        return VLC_EGENERIC;
    p_sys->f_volume = volume;
    aout_VolumeReport(p_aout, volume);
    /* Set volume for output unit */
    if(!p_sys->b_mute) {
        ostatus = AudioUnitSetParameter(p_sys->au_unit,
                                        kHALOutputParam_Volume,
                                        kAudioUnitScope_Global,
                                        0,
                                        volume * volume * volume,
                                        0);
    }
    if (var_InheritBool(p_aout, "volume-save"))
        config_PutInt(p_aout, "auhal-volume", lroundf(volume * AOUT_VOLUME_DEFAULT));
    return ostatus;
}
