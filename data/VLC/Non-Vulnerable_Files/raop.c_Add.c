 *****************************************************************************/
static sout_stream_id_sys_t *Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_id_sys_t *id = NULL;
    id = calloc( 1, sizeof( *id ) );
    if ( id == NULL )
        goto error;
    es_format_Copy( &id->fmt, p_fmt );
    switch ( id->fmt.i_cat )
    {
    case AUDIO_ES:
        if ( id->fmt.i_codec == VLC_CODEC_ALAC )
        {
            if ( p_sys->p_audio_stream )
            {
                msg_Warn( p_stream, "Only the first Apple Lossless audio "
                                    "stream is used" );
            }
            else if ( id->fmt.audio.i_rate != 44100 ||
                      id->fmt.audio.i_channels != 2 )
            {
                msg_Err( p_stream, "The Apple Lossless audio stream must be "
                                   "encoded with 44100 Hz and 2 channels" );
            }
            else
            {
                /* Use this stream */
                p_sys->p_audio_stream = id;
            }
        }
        else if ( !p_sys->b_alac_warning )
        {
            msg_Err( p_stream, "Apple Lossless is the only codec supported. "
                               "Use the \"transcode\" module for conversion "
                               "(e.g. \"transcode{acodec=alac,"
                               "channels=2}\")." );
            p_sys->b_alac_warning = true;
        }
        break;
    default:
        /* Leave other stream types alone */
        break;
    }
    return id;
error:
    FreeId( id );
    return NULL;
}
