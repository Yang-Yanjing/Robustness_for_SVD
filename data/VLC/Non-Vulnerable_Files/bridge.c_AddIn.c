};
static sout_stream_id_sys_t * AddIn( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    in_sout_stream_sys_t *p_sys = (in_sout_stream_sys_t *)p_stream->p_sys;
    sout_stream_id_sys_t *id = malloc( sizeof( sout_stream_id_sys_t ) );
    if( !id ) return NULL;
    id->id = p_stream->p_next->pf_add( p_stream->p_next, p_fmt );
    if( !id->id )
    {
        free( id );
        return NULL;
    }
    if( p_sys->b_placeholder )
    {
        id->i_cat = p_fmt->i_cat;
        switch( p_fmt->i_cat )
        {
            case VIDEO_ES:
                if( p_sys->id_video != NULL )
                    msg_Err( p_stream, "We already had a video es!" );
                p_sys->id_video = id->id;
                break;
            case AUDIO_ES:
                if( p_sys->id_audio != NULL )
                    msg_Err( p_stream, "We already had an audio es!" );
                p_sys->id_audio = id->id;
                break;
        }
    }
    return id;
}
