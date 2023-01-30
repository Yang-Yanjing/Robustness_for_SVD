}
static int MuteSet( audio_output_t *p_aout, bool mute )
{
    HRESULT res = DS_OK;
    aout_sys_t *sys = p_aout->sys;
    sys->volume.mute = mute;
    if( sys->s.p_dsbuffer != NULL )
        res = IDirectSoundBuffer_SetVolume( sys->s.p_dsbuffer,
                                            mute? DSBVOLUME_MIN : sys->volume.mb );
    aout_MuteReport( p_aout, mute );
    return (res != DS_OK);
}
