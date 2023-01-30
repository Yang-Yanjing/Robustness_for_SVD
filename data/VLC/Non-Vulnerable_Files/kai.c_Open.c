}
static int Open (vlc_object_t *obj)
{
    audio_output_t *aout = (audio_output_t *)obj;
    aout_sys_t *sys = calloc( 1, sizeof( aout_sys_t ) );
    if( unlikely( sys == NULL ))
        return VLC_ENOMEM;
    aout->sys = sys;
    aout->start = Start;
    aout->stop = Stop;
    aout_SoftVolumeInit( aout );
    return VLC_SUCCESS;
}
