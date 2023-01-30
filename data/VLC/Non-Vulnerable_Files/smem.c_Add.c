}
static sout_stream_id_sys_t *Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_id_sys_t *id = NULL;
    if ( p_fmt->i_cat == VIDEO_ES )
        id = AddVideo( p_stream, p_fmt );
    else if ( p_fmt->i_cat == AUDIO_ES )
        id = AddAudio( p_stream, p_fmt );
    return id;
}
