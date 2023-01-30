 *****************************************************************************/
static sout_stream_id_sys_t * Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_id_sys_t  *id;
    int i;
    /* search a compatible output */
    for( i = 0; i < p_sys->i_id; i++ )
    {
        id = p_sys->id[i];
        if( id->b_used )
            continue;
        if( id->fmt.i_cat != p_fmt->i_cat || id->fmt.i_codec != p_fmt->i_codec )
            continue;
        if( id->fmt.i_cat == AUDIO_ES )
        {
            audio_format_t *p_a = &id->fmt.audio;
            if( p_a->i_rate != p_fmt->audio.i_rate ||
                p_a->i_channels != p_fmt->audio.i_channels ||
                p_a->i_blockalign != p_fmt->audio.i_blockalign )
                continue;
        }
        else if( id->fmt.i_cat == VIDEO_ES )
        {
            video_format_t *p_v = &id->fmt.video;
            if( p_v->i_width != p_fmt->video.i_width ||
                p_v->i_height != p_fmt->video.i_height )
                continue;
        }
        /* */
        msg_Dbg( p_stream, "reusing already opened output" );
        id->b_used = true;
        id->b_streamswap = true;
        return id;
    }
    /* destroy all outputs from the same category */
    for( i = 0; i < p_sys->i_id; i++ )
    {
        id = p_sys->id[i];
        if( !id->b_used && id->fmt.i_cat == p_fmt->i_cat )
        {
            TAB_REMOVE( p_sys->i_id, p_sys->id, id );
            sout_StreamIdDel( p_stream->p_next, id->id );
            es_format_Clean( &id->fmt );
            free( id );
            i = 0;
            continue;
        }
    }
    msg_Dbg( p_stream, "creating new output" );
    id = malloc( sizeof( sout_stream_id_sys_t ) );
    if( id == NULL )
        return NULL;
    es_format_Copy( &id->fmt, p_fmt );
    id->b_used           = true;
    id->id               = sout_StreamIdAdd( p_stream->p_next, &id->fmt );
    if( id->id == NULL )
    {
        free( id );
        return NULL;
    }
    TAB_APPEND( p_sys->i_id, p_sys->id, id );
    return id;
}
