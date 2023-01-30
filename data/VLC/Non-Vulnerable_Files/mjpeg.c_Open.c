 *****************************************************************************/
static int Open( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    int         i_size;
    bool        b_matched = false;
    if( IsMxpeg( p_demux->s ) && !p_demux->b_force )
        // let avformat handle this case
        return VLC_EGENERIC;
    demux_sys_t *p_sys = malloc( sizeof( demux_sys_t ) );
    if( unlikely(p_sys == NULL) )
        return VLC_ENOMEM;
    p_demux->pf_control = Control;
    p_demux->p_sys      = p_sys;
    p_sys->p_es         = NULL;
    p_sys->i_time       = VLC_TS_0;
    p_sys->i_level      = 0;
    p_sys->psz_separator = NULL;
    p_sys->i_frame_size_estimate = 15 * 1024;
    char *content_type = stream_ContentType( p_demux->s );
    if ( content_type )
    {
        //FIXME: this is not fully match to RFC
        char* boundary = strstr( content_type, "boundary=" );
        if( boundary )
        {
	    boundary += strlen( "boundary=" );
	    size_t len = strlen( boundary );
	    if( len > 2 && boundary[0] == '"'
	        && boundary[len-1] == '"' )
	    {
	        boundary[len-1] = '\0';
	        boundary++;
	    }
            p_sys->psz_separator = strdup( boundary );
            if( !p_sys->psz_separator )
            {
                free( content_type );
                goto error;
            }
        }
        free( content_type );
    }
    b_matched = CheckMimeHeader( p_demux, &i_size);
    if( b_matched )
    {
        p_demux->pf_demux = MimeDemux;
        stream_Read( p_demux->s, NULL, i_size );
    }
    else if( i_size == 0 )
    {
        /* 0xffd8 identify a JPEG SOI */
        if( p_sys->p_peek[0] == 0xFF && p_sys->p_peek[1] == 0xD8 )
        {
            msg_Dbg( p_demux, "JPEG SOI marker detected" );
            p_demux->pf_demux = MjpgDemux;
            p_sys->i_level++;
        }
        else
        {
            goto error;
        }
    }
    else
    {
        goto error;
    }
    /* Frame rate */
    float f_fps = var_InheritFloat( p_demux, "mjpeg-fps" );
    p_sys->i_still_end = 0;
    if( demux_IsPathExtension( p_demux, ".jpeg" ) ||
        demux_IsPathExtension( p_demux, ".jpg" ) )
    {
        /* Plain JPEG file = single still picture */
        p_sys->b_still = true;
        if( f_fps == 0.f )
            /* Defaults to 1fps */
            f_fps = 1.f;
    }
    else
        p_sys->b_still = false;
    p_sys->i_frame_length = f_fps ? (CLOCK_FREQ / f_fps) : 0;
    es_format_Init( &p_sys->fmt, VIDEO_ES, 0 );
    p_sys->fmt.i_codec = VLC_CODEC_MJPG;
    p_sys->p_es = es_out_Add( p_demux->out, &p_sys->fmt );
    return VLC_SUCCESS;
error:
    free( p_sys->psz_separator );
    free( p_sys );
    return VLC_EGENERIC;
}
