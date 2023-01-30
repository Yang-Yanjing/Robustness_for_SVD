}
static void Pause( audio_output_t *aout, bool pause, mtime_t date )
{
    VLC_UNUSED( date );
    aout_sys_t *sys = aout->sys;
    if( pause )
        kaiPause( sys->hkai );
    else
        kaiResume( sys->hkai );
}
