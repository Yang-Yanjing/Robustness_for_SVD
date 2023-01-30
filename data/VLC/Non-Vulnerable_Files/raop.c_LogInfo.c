}
static void LogInfo( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    const char *psz_jack_name;
    msg_Info( p_this, "Audio latency: %d", p_sys->i_audio_latency );
    switch ( p_sys->i_jack_type )
    {
        case JACK_TYPE_ANALOG:
            psz_jack_name = "analog";
            break;
        case JACK_TYPE_DIGITAL:
            psz_jack_name = "digital";
            break;
        case JACK_TYPE_NONE:
        default:
            psz_jack_name = "none";
            break;
    }
    msg_Info( p_this, "Jack type: %s", psz_jack_name );
}
