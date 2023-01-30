}
static sout_stream_id_sys_t * Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    if ( p_fmt->i_id == p_sys->i_id )
    {
        p_sys->psz_old_language = p_fmt->psz_language;
        msg_Dbg( p_stream,
                 "changing language of ID %d (magazine %d page %x row %d)",
                 p_sys->i_id, p_sys->i_magazine, p_sys->i_page, p_sys->i_row );
        p_sys->psz_language = p_fmt->psz_language = malloc(4);
        if ( p_sys->psz_old_language != NULL )
            strncpy( p_fmt->psz_language, p_sys->psz_old_language, 3 );
        else
            strcpy( p_fmt->psz_language, "unk" );
        p_fmt->psz_language[3] = '\0';
        p_sys->p_id = p_stream->p_next->pf_add( p_stream->p_next, p_fmt );
        return p_sys->p_id;
    }
    if ( p_fmt->i_codec == VLC_CODEC_TELETEXT )
    {
        p_sys->p_telx = p_stream->p_next->pf_add( p_stream->p_next, p_fmt );
        return p_sys->p_telx;
    }
    return p_stream->p_next->pf_add( p_stream->p_next, p_fmt );
}
