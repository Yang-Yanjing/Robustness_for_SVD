}
static int AddStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    avi_stream_t    *p_stream;
    if( p_sys->i_streams >= 100 )
    {
        msg_Err( p_mux, "too many streams" );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_mux, "adding input" );
    p_input->p_sys = malloc( sizeof( int ) );
    if( !p_input->p_sys )
        return VLC_ENOMEM;
    *((int*)p_input->p_sys) = p_sys->i_streams;
    p_stream = &p_sys->stream[p_sys->i_streams];
    switch( p_input->p_fmt->i_cat )
    {
        case AUDIO_ES:
            p_stream->i_cat = AUDIO_ES;
            p_stream->fcc[0] = '0' + p_sys->i_streams / 10;
            p_stream->fcc[1] = '0' + p_sys->i_streams % 10;
            p_stream->fcc[2] = 'w';
            p_stream->fcc[3] = 'b';
            p_stream->p_bih = NULL;
            p_stream->p_wf  = malloc( sizeof( WAVEFORMATEX ) +
                                      p_input->p_fmt->i_extra );
            if( !p_stream->p_wf )
            {
                free( p_input->p_sys );
                return VLC_ENOMEM;
            }
#define p_wf p_stream->p_wf
            p_wf->cbSize = p_input->p_fmt->i_extra;
            if( p_wf->cbSize > 0 )
            {
                memcpy( &p_wf[1],
                        p_input->p_fmt->p_extra,
                        p_input->p_fmt->i_extra );
            }
            p_wf->nChannels      = p_input->p_fmt->audio.i_channels;
            p_wf->nSamplesPerSec = p_input->p_fmt->audio.i_rate;
            p_wf->nBlockAlign    = p_input->p_fmt->audio.i_blockalign;
            p_wf->nAvgBytesPerSec= p_input->p_fmt->i_bitrate / 8;
            p_wf->wBitsPerSample = 0;
            switch( p_input->p_fmt->i_codec )
            {
                case VLC_CODEC_A52:
                    p_wf->wFormatTag = WAVE_FORMAT_A52;
                    p_wf->nBlockAlign= 1;
                    break;
                case VLC_CODEC_MP3:
                    p_wf->wFormatTag = WAVE_FORMAT_MPEGLAYER3;
                    p_wf->nBlockAlign= 1;
                    break;
                case VLC_CODEC_WMA1:
                    p_wf->wFormatTag = WAVE_FORMAT_WMA1;
                    break;
                case VLC_CODEC_WMA2:
                    p_wf->wFormatTag = WAVE_FORMAT_WMA2;
                    break;
                case VLC_CODEC_WMAP:
                    p_wf->wFormatTag = WAVE_FORMAT_WMAP;
                    break;
                case VLC_CODEC_WMAL:
                    p_wf->wFormatTag = WAVE_FORMAT_WMAL;
                    break;
                    /* raw codec */
                case VLC_CODEC_U8:
                    p_wf->wFormatTag = WAVE_FORMAT_PCM;
                    p_wf->nBlockAlign= p_wf->nChannels;
                    p_wf->wBitsPerSample = 8;
                    p_wf->nAvgBytesPerSec = (p_wf->wBitsPerSample/8) *
                                      p_wf->nSamplesPerSec * p_wf->nChannels;
                    break;
                case VLC_CODEC_S16L:
                    p_wf->wFormatTag = WAVE_FORMAT_PCM;
                    p_wf->nBlockAlign= 2 * p_wf->nChannels;
                    p_wf->wBitsPerSample = 16;
                    p_wf->nAvgBytesPerSec = (p_wf->wBitsPerSample/8) *
                                      p_wf->nSamplesPerSec * p_wf->nChannels;
                    break;
                case VLC_CODEC_S24L:
                    p_wf->wFormatTag = WAVE_FORMAT_PCM;
                    p_wf->nBlockAlign= 3 * p_wf->nChannels;
                    p_wf->wBitsPerSample = 24;
                    p_wf->nAvgBytesPerSec = (p_wf->wBitsPerSample/8) *
                                      p_wf->nSamplesPerSec * p_wf->nChannels;
                    break;
                case VLC_CODEC_S32L:
                    p_wf->wFormatTag = WAVE_FORMAT_PCM;
                    p_wf->nBlockAlign= 4 * p_wf->nChannels;
                    p_wf->wBitsPerSample = 32;
                    p_wf->nAvgBytesPerSec = (p_wf->wBitsPerSample/8) *
                                      p_wf->nSamplesPerSec * p_wf->nChannels;
                    break;
                default:
                    return VLC_EGENERIC;
            }
#undef p_wf
            break;
        case VIDEO_ES:
            p_stream->i_cat = VIDEO_ES;
            p_stream->fcc[0] = '0' + p_sys->i_streams / 10;
            p_stream->fcc[1] = '0' + p_sys->i_streams % 10;
            p_stream->fcc[2] = 'd';
            p_stream->fcc[3] = 'c';
            if( p_sys->i_stream_video < 0 )
            {
                p_sys->i_stream_video = p_sys->i_streams;
            }
            p_stream->p_wf  = NULL;
            p_stream->p_bih = malloc( sizeof( VLC_BITMAPINFOHEADER ) +
                                      p_input->p_fmt->i_extra );
            if( !p_stream->p_bih )
            {
                free( p_input->p_sys );
                return VLC_ENOMEM;
            }
#define p_bih p_stream->p_bih
            p_bih->biSize  = sizeof( VLC_BITMAPINFOHEADER ) +
                             p_input->p_fmt->i_extra;
            if( p_input->p_fmt->i_extra > 0 )
            {
                memcpy( &p_bih[1],
                        p_input->p_fmt->p_extra,
                        p_input->p_fmt->i_extra );
            }
            p_bih->biWidth = p_input->p_fmt->video.i_width;
            p_bih->biHeight= p_input->p_fmt->video.i_height;
            p_bih->biPlanes= 1;
            p_bih->biBitCount       = 24;
            p_bih->biSizeImage      = 0;
            p_bih->biXPelsPerMeter  = 0;
            p_bih->biYPelsPerMeter  = 0;
            p_bih->biClrUsed        = 0;
            p_bih->biClrImportant   = 0;
            switch( p_input->p_fmt->i_codec )
            {
                case VLC_CODEC_MP4V:
                    p_bih->biCompression = VLC_FOURCC( 'X', 'V', 'I', 'D' );
                    break;
                default:
                    p_bih->biCompression = p_input->p_fmt->i_original_fourcc ?: p_input->p_fmt->i_codec;
                    break;
            }
#undef p_bih
            break;
        default:
            return( VLC_EGENERIC );
    }
    p_stream->i_totalsize = 0;
    p_stream->i_frames    = 0;
    p_stream->i_duration  = 0;
    /* fixed later */
    p_stream->f_fps = 25;
    p_stream->i_bitrate = 128 * 1024;
    p_sys->i_streams++;
    return( VLC_SUCCESS );
}
