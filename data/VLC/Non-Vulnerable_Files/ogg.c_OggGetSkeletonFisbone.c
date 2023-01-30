static void OggGetSkeletonFisbone( uint8_t **pp_buffer, long *pi_size,
                                   sout_input_t *p_input, sout_mux_t *p_mux )
{
    uint8_t *psz_header;
    uint8_t *p_buffer;
    const char *psz_value = NULL;
    ogg_stream_t *p_stream = (ogg_stream_t *) p_input->p_sys;
    struct
    {
        char *psz_content_type;
        char *psz_role;
        long int i_size;
        unsigned int i_count;
    } headers = { NULL, NULL, 0, 0 };
    *pi_size = 0;
    switch( p_stream->i_fourcc )
    {
        case VLC_CODEC_VORBIS:
            psz_value = "audio/vorbis";
            break;
        case VLC_CODEC_THEORA:
            psz_value = "video/theora";
            break;
        case VLC_CODEC_SPEEX:
            psz_value = "audio/speex";
            break;
        case VLC_CODEC_FLAC:
            psz_value = "audio/flac";
            break;
        case VLC_CODEC_CMML:
            psz_value = "text/cmml";
            break;
        case VLC_CODEC_KATE:
            psz_value = "application/kate";
            break;
        case VLC_CODEC_VP8:
            psz_value = "video/x-vp8";
            break;
        default:
            psz_value = "application/octet-stream";
            msg_Warn( p_mux, "Unknown fourcc for stream %s, setting Content-Type to %s",
                  vlc_fourcc_GetDescription( p_stream->i_cat, p_stream->i_fourcc ),
                  psz_value );
    }
    /* Content Type Header */
    if ( asprintf( &headers.psz_content_type, "Content-Type: %s\r\n", psz_value ) != -1 )
    {
        headers.i_size += strlen( headers.psz_content_type );
        headers.i_count++;
    }
    /* Set Role Header */
    if ( p_input->p_fmt->i_priority > ES_PRIORITY_NOT_SELECTABLE )
    {
        int i_max_prio = ES_PRIORITY_MIN;
        for ( int i=0; i< p_mux->i_nb_inputs; i++ )
        {
            if ( p_mux->pp_inputs[i]->p_fmt->i_cat != p_input->p_fmt->i_cat ) continue;
            i_max_prio = __MAX( p_mux->pp_inputs[i]->p_fmt->i_priority, i_max_prio );
        }
        psz_value = NULL;
        if ( p_input->p_fmt->i_cat == AUDIO_ES || p_input->p_fmt->i_cat == VIDEO_ES )
        {
            if ( p_input->p_fmt->i_priority == i_max_prio && i_max_prio >= ES_PRIORITY_SELECTABLE_MIN )
                psz_value = ( p_input->p_fmt->i_cat == VIDEO_ES ) ?
                            "video/main" : "audio/main";
            else
                psz_value = ( p_input->p_fmt->i_cat == VIDEO_ES ) ?
                            "video/alternate" : "audio/alternate";
        }
        else if ( p_input->p_fmt->i_cat == SPU_ES )
        {
            psz_value = ( p_input->p_fmt->i_codec == VLC_CODEC_KATE ) ?
                        "text/karaoke" : "text/subtitle";
        }
        if ( psz_value && asprintf( &headers.psz_role, "Role: %s\r\n", psz_value ) != -1 )
        {
            headers.i_size += strlen( headers.psz_role );
            headers.i_count++;
        }
    }
    *pp_buffer = calloc( FISBONE_BASE_SIZE + headers.i_size, sizeof(uint8_t) );
    if ( !*pp_buffer ) return;
    p_buffer = *pp_buffer;
    memcpy( p_buffer, "fisbone", 8 );
    SetDWLE( &p_buffer[8], FISBONE_BASE_OFFSET ); /* offset to message headers */
    SetDWLE( &p_buffer[12], p_stream->i_serial_no );
    SetDWLE( &p_buffer[16], headers.i_count );
    /* granulerate den */
    switch ( p_input->p_fmt->i_cat )
    {
        case VIDEO_ES:
            SetQWLE( &(*pp_buffer)[20], p_input->p_fmt->video.i_frame_rate );
            SetQWLE( &(*pp_buffer)[28], p_input->p_fmt->video.i_frame_rate_base );
        break;
        case AUDIO_ES:
            SetQWLE( &(*pp_buffer)[20], p_input->p_fmt->audio.i_rate );
            SetQWLE( &(*pp_buffer)[28], 1 );
        break;
        default:
            SetQWLE( &(*pp_buffer)[20], 1000 );
            SetQWLE( &(*pp_buffer)[28], 1 );
    }
    /* preroll */
    if ( p_input->p_fmt->p_extra )
        SetDWLE( &(*pp_buffer)[44], xiph_CountHeaders( p_input->p_fmt->p_extra, p_input->p_fmt->i_extra ) );
    if ( headers.i_size > 0 )
    {
        psz_header = *pp_buffer + FISBONE_BASE_SIZE;
        memcpy( psz_header, headers.psz_content_type, strlen( headers.psz_content_type ) );
        psz_header += strlen( headers.psz_content_type );
        if ( headers.psz_role )
            memcpy( psz_header, headers.psz_role, strlen( headers.psz_role ) );
    }
    *pi_size = FISBONE_BASE_SIZE + headers.i_size;
    free( headers.psz_content_type );
    free( headers.psz_role );
}