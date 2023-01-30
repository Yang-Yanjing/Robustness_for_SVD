}
static int VolumeSet( audio_output_t *p_aout, float volume )
{
    aout_sys_t *sys = p_aout->sys;
    int ret = 0;
    /* Directsound doesn't support amplification, so we use software
       gain if we need it and only for this */
    float gain = volume > 1.f ? volume * volume * volume : 1.f;
    aout_GainRequest( p_aout, gain );
    /* millibels from linear amplification */
    LONG mb = lroundf( 6000.f * log10f( __MIN( volume, 1.f ) ));
    /* Clamp to allowed DirectSound range */
    static_assert( DSBVOLUME_MIN < DSBVOLUME_MAX, "DSBVOLUME_* confused" );
    if( mb > DSBVOLUME_MAX )
    {
        mb = DSBVOLUME_MAX;
        ret = -1;
    }
    if( mb <= DSBVOLUME_MIN )
        mb = DSBVOLUME_MIN;
    sys->volume.mb = mb;
    sys->volume.volume = volume;
    if( !sys->volume.mute && sys->s.p_dsbuffer != NULL &&
        IDirectSoundBuffer_SetVolume( sys->s.p_dsbuffer, mb ) != DS_OK )
        return -1;
    /* Convert back to UI volume */
    aout_VolumeReport( p_aout, volume );
    if( var_InheritBool( p_aout, "volume-save" ) )
        config_PutFloat( p_aout, "directx-volume", volume );
    return ret;
}
