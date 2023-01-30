}
static int MuteSet(audio_output_t * p_aout, bool mute)
{
    struct   aout_sys_t *p_sys = p_aout->sys;
    OSStatus ostatus;
    if(p_sys->b_digital)
        return VLC_EGENERIC;
    p_sys->b_mute = mute;
    aout_MuteReport(p_aout, mute);
    float volume = .0;
    if (!mute)
        volume = p_sys->f_volume;
    ostatus = AudioUnitSetParameter(p_sys->au_unit,
                                    kHALOutputParam_Volume,
                                    kAudioUnitScope_Global,
                                    0,
                                    volume * volume * volume,
                                    0);
    return ostatus;
}
