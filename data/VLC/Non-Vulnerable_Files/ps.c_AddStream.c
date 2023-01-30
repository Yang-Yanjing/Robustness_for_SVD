 *****************************************************************************/
static int AddStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    ps_stream_t *p_stream;
    msg_Dbg( p_mux, "adding input codec=%4.4s",
             (char*)&p_input->p_fmt->i_codec );
    p_input->p_sys = p_stream = malloc( sizeof( ps_stream_t ) );
    p_stream->i_stream_type = 0x81;
    /* Init this new stream */
    switch( p_input->p_fmt->i_codec )
    {
        case VLC_CODEC_MPGV:
        case VLC_CODEC_MP2V:
        case VLC_CODEC_MP1V:
            p_stream->i_stream_id =
                StreamIdGet( p_sys->stream_id_mpgv, 0xe0, 0xef );
            p_stream->i_stream_type = 0x02; /* ISO/IEC 13818 Video */
            break;
        case VLC_CODEC_MP4V:
            p_stream->i_stream_id =
                StreamIdGet( p_sys->stream_id_mpgv, 0xe0, 0xef );
            p_stream->i_stream_type = 0x10;
            break;
        case VLC_CODEC_H264:
            p_stream->i_stream_id =
                StreamIdGet( p_sys->stream_id_mpgv, 0xe0, 0xef );
            p_stream->i_stream_type = 0x1b;
            break;
        case VLC_CODEC_DVD_LPCM:
            p_stream->i_stream_id =
                0xbd00 | StreamIdGet( p_sys->stream_id_lpcm, 0xa0, 0xaf );
            break;
        case VLC_CODEC_DTS:
            p_stream->i_stream_id =
                0xbd00 | StreamIdGet( p_sys->stream_id_dts, 0x88, 0x8f );
            break;
        case VLC_CODEC_A52:
            p_stream->i_stream_id =
                0xbd00 | StreamIdGet( p_sys->stream_id_a52, 0x80, 0x87 );
            break;
        case VLC_CODEC_MPGA:
            p_stream->i_stream_id =
                StreamIdGet( p_sys->stream_id_mpga, 0xc0, 0xcf );
            p_stream->i_stream_type = 0x03; /* ISO/IEC 11172 Audio */
            break;
        case VLC_CODEC_MP4A:
            p_stream->i_stream_id =
                StreamIdGet( p_sys->stream_id_mpga, 0xc0, 0xcf );
            p_stream->i_stream_type = 0x0f;
            break;
        case VLC_CODEC_SPU:
            p_stream->i_stream_id =
                0xbd00 | StreamIdGet( p_sys->stream_id_spu, 0x20, 0x3f );
            break;
        default:
            goto error;
    }
    if( p_stream->i_stream_id < 0 ) goto error;
    if( p_input->p_fmt->i_cat == AUDIO_ES )
    {
        p_sys->i_audio_bound++;
        p_stream->i_max_buff_size = 4 * 1024;
    }
    else if( p_input->p_fmt->i_cat == VIDEO_ES )
    {
        p_sys->i_video_bound++;
        p_stream->i_max_buff_size = 400 * 1024; /* FIXME -- VCD uses 46, SVCD
                        uses 230, ffmpeg has 230 with a note that it is small */
    }
    else
    {   /* FIXME -- what's valid for not audio or video? */
        p_stream->i_max_buff_size = 4 * 1024;
    }
    /* Try to set a sensible default value for the instant bitrate */
    p_sys->i_instant_bitrate += p_input->p_fmt->i_bitrate + 1000/* overhead */;
    /* FIXME -- spec requires  an upper limit rate boundary in the system header;
       our codecs are VBR; using 2x nominal rate, convert to 50 bytes/sec */
    p_sys->i_rate_bound += p_input->p_fmt->i_bitrate * 2 / (8 * 50);
    p_sys->i_psm_version++;
    p_stream->lang[0] = p_stream->lang[1] = p_stream->lang[2] = 0;
    if( p_input->p_fmt->psz_language )
    {
        char *psz = p_input->p_fmt->psz_language;
        const iso639_lang_t *pl = NULL;
        if( strlen( psz ) == 2 )
        {
            pl = GetLang_1( psz );
        }
        else if( strlen( psz ) == 3 )
        {
            pl = GetLang_2B( psz );
            if( !strcmp( pl->psz_iso639_1, "??" ) )
            {
                pl = GetLang_2T( psz );
            }
        }
        if( pl && strcmp( pl->psz_iso639_1, "??" ) )
        {
            p_stream->lang[0] = pl->psz_iso639_2T[0];
            p_stream->lang[1] = pl->psz_iso639_2T[1];
            p_stream->lang[2] = pl->psz_iso639_2T[2];
            msg_Dbg( p_mux, "    - lang=%c%c%c",
                     p_stream->lang[0], p_stream->lang[1], p_stream->lang[2] );
        }
    }
    return VLC_SUCCESS;
error:
    free( p_stream );
    return VLC_EGENERIC;
}
