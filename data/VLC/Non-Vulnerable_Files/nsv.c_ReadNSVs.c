 *****************************************************************************/
static int ReadNSVs( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    uint8_t      header[19];
    vlc_fourcc_t fcc;
    if( stream_Read( p_demux->s, header, 19 ) < 19 )
    {
        msg_Warn( p_demux, "cannot read" );
        return VLC_EGENERIC;
    }
    /* Video */
    switch( ( fcc = VLC_FOURCC( header[4], header[5], header[6], header[7] ) ) )
    {
        case VLC_FOURCC( 'V', 'P', '3', ' ' ):
        case VLC_FOURCC( 'V', 'P', '3', '0' ):
            fcc = VLC_FOURCC( 'V', 'P', '3', '0' );
            break;
        case VLC_FOURCC( 'V', 'P', '3', '1' ):
            fcc = VLC_FOURCC( 'V', 'P', '3', '1' );
            break;
        case VLC_FOURCC( 'V', 'P', '5', ' ' ):
        case VLC_FOURCC( 'V', 'P', '5', '0' ):
            fcc = VLC_FOURCC( 'V', 'P', '5', '0' );
            break;
        case VLC_FOURCC( 'V', 'P', '6', '0' ):
        case VLC_FOURCC( 'V', 'P', '6', '1' ):
        case VLC_FOURCC( 'V', 'P', '6', '2' ):
        case VLC_FOURCC( 'V', 'P', '8', '0' ):
        case VLC_FOURCC( 'H', '2', '6', '4' ):
        case VLC_FOURCC( 'N', 'O', 'N', 'E' ):
            break;
        default:
            msg_Warn( p_demux, "unknown codec %4.4s", (char *)&fcc );
            break;
    }
    if( fcc != VLC_FOURCC( 'N', 'O', 'N', 'E' ) && fcc != p_sys->fmt_video.i_codec  )
    {
        es_format_Init( &p_sys->fmt_video, VIDEO_ES, fcc );
        p_sys->fmt_video.video.i_width = GetWLE( &header[12] );
        p_sys->fmt_video.video.i_height = GetWLE( &header[14] );
        if( p_sys->p_video )
        {
            es_out_Del( p_demux->out, p_sys->p_video );
        }
        p_sys->p_video = es_out_Add( p_demux->out, &p_sys->fmt_video );
        msg_Dbg( p_demux, "    - video `%4.4s' %dx%d",
                 (char*)&fcc,
                 p_sys->fmt_video.video.i_width,
                 p_sys->fmt_video.video.i_height );
    }
    /* Audio */
    switch( ( fcc = VLC_FOURCC( header[8], header[9], header[10], header[11] ) ) )
    {
        case VLC_FOURCC( 'M', 'P', '3', ' ' ):
            fcc = VLC_FOURCC( 'm', 'p', 'g', 'a' );
            break;
        case VLC_FOURCC( 'P', 'C', 'M', ' ' ):
            fcc = VLC_FOURCC( 'a', 'r', 'a', 'w' );
            break;
        case VLC_FOURCC( 'A', 'A', 'C', ' ' ):
        case VLC_FOURCC( 'A', 'A', 'C', 'P' ):
            fcc = VLC_FOURCC( 'm', 'p', '4', 'a' );
            break;
        case VLC_FOURCC( 'S', 'P', 'X', ' ' ):
            fcc = VLC_FOURCC( 's', 'p', 'x', ' ' );
            break;
        case VLC_FOURCC( 'N', 'O', 'N', 'E' ):
            break;
        default:
            msg_Warn( p_demux, "unknown codec %4.4s", (char *)&fcc );
            break;
    }
    if( fcc != VLC_FOURCC( 'N', 'O', 'N', 'E' ) && fcc != p_sys->fmt_audio.i_codec )
    {
        msg_Dbg( p_demux, "    - audio `%4.4s'", (char*)&fcc );
        if( p_sys->p_audio )
        {
            es_out_Del( p_demux->out, p_sys->p_audio );
            p_sys->p_audio = NULL;
        }
        es_format_Init( &p_sys->fmt_audio, AUDIO_ES, fcc );
    }
    if( header[16]&0x80 )
    {
        /* Fractional frame rate */
        switch( header[16]&0x03 )
        {
            case 0: /* 30 fps */
                p_sys->i_pcr_inc = 33333; /* 300000/9 */
                break;
            case 1: /* 29.97 fps */
                p_sys->i_pcr_inc = 33367; /* 300300/9 */
                break;
            case 2: /* 25 fps */
                p_sys->i_pcr_inc = 40000; /* 360000/9 */
                break;
            case 3: /* 23.98 fps */
                p_sys->i_pcr_inc = 41700; /* 375300/9 */
                break;
        }
        if( header[16] < 0xc0 )
            p_sys->i_pcr_inc = p_sys->i_pcr_inc * (((header[16] ^ 0x80) >> 2 ) +1 );
        else
            p_sys->i_pcr_inc = p_sys->i_pcr_inc / (((header[16] ^ 0xc0) >> 2 ) +1 );
    }
    else if( header[16] != 0 )
    {
        /* Integer frame rate */
        p_sys->i_pcr_inc = 1000000 / header[16];
    }
    else
    {
        msg_Dbg( p_demux, "invalid fps (0x00)" );
        p_sys->i_pcr_inc = 40000;
    }
    //msg_Dbg( p_demux, "    - fps=%.3f", 1000000.0 / (double)p_sys->i_pcr_inc );
    return VLC_SUCCESS;
}
