 *****************************************************************************/
static int DecOpen( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    DMO_MEDIA_TYPE dmo_input_type, dmo_output_type;
    IMediaObject *p_dmo = NULL;
    HINSTANCE hmsdmo_dll = NULL;
    VIDEOINFOHEADER *p_vih = NULL;
    WAVEFORMATEX *p_wf = NULL;
    /* Initialize OLE/COM */
    CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
    if( LoadDMO( VLC_OBJECT(p_dec), &hmsdmo_dll, &p_dmo, &p_dec->fmt_in, false )
        != VLC_SUCCESS )
    {
        hmsdmo_dll = 0;
        p_dmo = 0;
        goto error;
    }
    /* Setup input format */
    memset( &dmo_input_type, 0, sizeof(dmo_input_type) );
    dmo_input_type.pUnk = 0;
    if( p_dec->fmt_in.i_cat == AUDIO_ES )
    {
        uint16_t i_tag;
        int i_size = sizeof(WAVEFORMATEX) + p_dec->fmt_in.i_extra;
        p_wf = malloc( i_size );
        memset( p_wf, 0, sizeof(WAVEFORMATEX) );
        if( p_dec->fmt_in.i_extra )
            memcpy( &p_wf[1], p_dec->fmt_in.p_extra, p_dec->fmt_in.i_extra );
        dmo_input_type.majortype  = MEDIATYPE_Audio;
        dmo_input_type.subtype    = dmo_input_type.majortype;
        dmo_input_type.subtype.Data1 = p_dec->fmt_in.i_original_fourcc ?: p_dec->fmt_in.i_codec;
        fourcc_to_wf_tag( p_dec->fmt_in.i_codec, &i_tag );
        if( i_tag ) dmo_input_type.subtype.Data1 = i_tag;
        p_wf->wFormatTag = dmo_input_type.subtype.Data1;
        p_wf->nSamplesPerSec = p_dec->fmt_in.audio.i_rate;
        p_wf->nChannels = p_dec->fmt_in.audio.i_channels;
        p_wf->wBitsPerSample = p_dec->fmt_in.audio.i_bitspersample;
        p_wf->nBlockAlign = p_dec->fmt_in.audio.i_blockalign;
        p_wf->nAvgBytesPerSec = p_dec->fmt_in.i_bitrate / 8;
        p_wf->cbSize = p_dec->fmt_in.i_extra;
        dmo_input_type.formattype = FORMAT_WaveFormatEx;
        dmo_input_type.cbFormat   = i_size;
        dmo_input_type.pbFormat   = (char *)p_wf;
        dmo_input_type.bFixedSizeSamples = 1;
        dmo_input_type.bTemporalCompression = 0;
        dmo_input_type.lSampleSize = p_wf->nBlockAlign;
    }
    else
    {
        VLC_BITMAPINFOHEADER *p_bih;
        int i_size = sizeof(VIDEOINFOHEADER) + p_dec->fmt_in.i_extra;
        p_vih = malloc( i_size );
        memset( p_vih, 0, sizeof(VIDEOINFOHEADER) );
        if( p_dec->fmt_in.i_extra )
            memcpy( &p_vih[1], p_dec->fmt_in.p_extra, p_dec->fmt_in.i_extra );
        p_bih = &p_vih->bmiHeader;
        p_bih->biCompression = p_dec->fmt_in.i_original_fourcc ?: p_dec->fmt_in.i_codec;
        p_bih->biWidth = p_dec->fmt_in.video.i_width;
        p_bih->biHeight = p_dec->fmt_in.video.i_height;
        p_bih->biBitCount = p_dec->fmt_in.video.i_bits_per_pixel;
        p_bih->biPlanes = 1;
        p_bih->biSize = i_size - sizeof(VIDEOINFOHEADER) +
            sizeof(VLC_BITMAPINFOHEADER);
        p_vih->rcSource.left = p_vih->rcSource.top = 0;
        p_vih->rcSource.right = p_dec->fmt_in.video.i_width;
        p_vih->rcSource.bottom = p_dec->fmt_in.video.i_height;
        p_vih->rcTarget = p_vih->rcSource;
        dmo_input_type.majortype  = MEDIATYPE_Video;
        dmo_input_type.subtype    = dmo_input_type.majortype;
        dmo_input_type.subtype.Data1 = p_dec->fmt_in.i_original_fourcc ?: p_dec->fmt_in.i_codec;
        dmo_input_type.formattype = FORMAT_VideoInfo;
        dmo_input_type.bFixedSizeSamples = 0;
        dmo_input_type.bTemporalCompression = 1;
        dmo_input_type.cbFormat = i_size;
        dmo_input_type.pbFormat = (char *)p_vih;
    }
    if( p_dmo->vt->SetInputType( p_dmo, 0, &dmo_input_type, 0 ) )
    {
        msg_Err( p_dec, "can't set DMO input type" );
        goto error;
    }
    msg_Dbg( p_dec, "DMO input type set" );
    /* Setup output format */
    memset( &dmo_output_type, 0, sizeof(dmo_output_type) );
    dmo_output_type.pUnk = 0;
    if( p_dec->fmt_in.i_cat == AUDIO_ES )
    {
        /* Setup the format */
        p_dec->fmt_out.i_codec = VLC_CODEC_S16N;
        p_dec->fmt_out.audio.i_rate     = p_dec->fmt_in.audio.i_rate;
        p_dec->fmt_out.audio.i_channels = p_dec->fmt_in.audio.i_channels;
        p_dec->fmt_out.audio.i_bitspersample = 16;//p_dec->fmt_in.audio.i_bitspersample; We request 16
        p_dec->fmt_out.audio.i_physical_channels =
            p_dec->fmt_out.audio.i_original_channels =
                pi_channels_maps[p_dec->fmt_out.audio.i_channels];
        p_wf->wFormatTag = WAVE_FORMAT_PCM;
        p_wf->nSamplesPerSec = p_dec->fmt_out.audio.i_rate;
        p_wf->nChannels = p_dec->fmt_out.audio.i_channels;
        p_wf->wBitsPerSample = p_dec->fmt_out.audio.i_bitspersample;
        p_wf->nBlockAlign =
            p_wf->wBitsPerSample / 8 * p_wf->nChannels;
        p_wf->nAvgBytesPerSec =
            p_wf->nSamplesPerSec * p_wf->nBlockAlign;
        p_wf->cbSize = 0;
        dmo_output_type.majortype  = MEDIATYPE_Audio;
        dmo_output_type.formattype = FORMAT_WaveFormatEx;
        dmo_output_type.subtype    = MEDIASUBTYPE_PCM;
        dmo_output_type.cbFormat   = sizeof(WAVEFORMATEX);
        dmo_output_type.pbFormat   = (char *)p_wf;
        dmo_output_type.bFixedSizeSamples = 1;
        dmo_output_type.bTemporalCompression = 0;
        dmo_output_type.lSampleSize = p_wf->nBlockAlign;
    }
    else
    {
        VLC_BITMAPINFOHEADER *p_bih;
        DMO_MEDIA_TYPE mt;
        unsigned i_chroma = VLC_CODEC_YUYV;
        int i_bpp = 16;
        int i = 0;
        /* Find out which chroma to use */
        while( !p_dmo->vt->GetOutputType( p_dmo, 0, i++, &mt ) )
        {
            if( mt.subtype.Data1 == VLC_CODEC_YV12 )
            {
                i_chroma = mt.subtype.Data1;
                i_bpp = 12;
                DMOFreeMediaType( &mt );
                break;
            }
            else if( (p_dec->fmt_in.i_codec == VLC_CODEC_MSS1 ||
                      p_dec->fmt_in.i_codec == VLC_CODEC_MSS2 ) &&
                      guidcmp( &mt.subtype, &MEDIASUBTYPE_RGB24 ) )
            {
                i_chroma = VLC_CODEC_RGB24;
                i_bpp = 24;
            }
            DMOFreeMediaType( &mt );
        }
        p_dec->fmt_out.i_codec = i_chroma == VLC_CODEC_YV12 ? VLC_CODEC_I420 : i_chroma;
        p_dec->fmt_out.video.i_width = p_dec->fmt_in.video.i_width;
        p_dec->fmt_out.video.i_height = p_dec->fmt_in.video.i_height;
        p_dec->fmt_out.video.i_bits_per_pixel = i_bpp;
        /* If an aspect-ratio was specified in the input format then force it */
        if( p_dec->fmt_in.video.i_sar_num > 0 &&
            p_dec->fmt_in.video.i_sar_den > 0 )
        {
            p_dec->fmt_out.video.i_sar_num = p_dec->fmt_in.video.i_sar_num;
            p_dec->fmt_out.video.i_sar_den = p_dec->fmt_in.video.i_sar_den;
        }
        else
        {
            p_dec->fmt_out.video.i_sar_num = 1;
            p_dec->fmt_out.video.i_sar_den = 1;
        }
        p_bih = &p_vih->bmiHeader;
        p_bih->biCompression = i_chroma == VLC_CODEC_RGB24 ? BI_RGB : i_chroma;
        p_bih->biHeight *= -1;
        p_bih->biBitCount = p_dec->fmt_out.video.i_bits_per_pixel;
        p_bih->biSizeImage = p_dec->fmt_in.video.i_width *
            p_dec->fmt_in.video.i_height *
            (p_dec->fmt_in.video.i_bits_per_pixel + 7) / 8;
        p_bih->biPlanes = 1; /* http://msdn.microsoft.com/en-us/library/dd183376%28v=vs.85%29.aspx */
        p_bih->biSize = sizeof(VLC_BITMAPINFOHEADER);
        dmo_output_type.majortype = MEDIATYPE_Video;
        dmo_output_type.formattype = FORMAT_VideoInfo;
        if( i_chroma == VLC_CODEC_RGB24 )
        {
            dmo_output_type.subtype = MEDIASUBTYPE_RGB24;
        }
        else
        {
            dmo_output_type.subtype = dmo_output_type.majortype;
            dmo_output_type.subtype.Data1 = p_bih->biCompression;
        }
        dmo_output_type.bFixedSizeSamples = true;
        dmo_output_type.bTemporalCompression = 0;
        dmo_output_type.lSampleSize = p_bih->biSizeImage;
        dmo_output_type.cbFormat = sizeof(VIDEOINFOHEADER);
        dmo_output_type.pbFormat = (char *)p_vih;
    }
#ifdef DMO_DEBUG
    /* Enumerate output types */
    if( p_dec->fmt_in.i_cat == VIDEO_ES )
    {
        int i = 0;
        DMO_MEDIA_TYPE mt;
        while( !p_dmo->vt->GetOutputType( p_dmo, 0, i++, &mt ) )
        {
            msg_Dbg( p_dec, "available output chroma: %4.4s", (char *)&mt.subtype.Data1 );
            DMOFreeMediaType( &mt );
        }
    }
#endif
    unsigned i_err = p_dmo->vt->SetOutputType( p_dmo, 0, &dmo_output_type, 0 );
    if( i_err )
    {
        msg_Err( p_dec, "can't set DMO output type for decoder: 0x%x", i_err );
        goto error;
    }
    msg_Dbg( p_dec, "DMO output type set for decoder" );
    /* Allocate the memory needed to store the decoder's structure */
    p_sys->hmsdmo_dll = hmsdmo_dll;
    p_sys->p_dmo = p_dmo;
    /* Find out some properties of the output */
    {
        uint32_t i_size, i_align;
        p_sys->i_min_output = 0;
        if( p_dmo->vt->GetOutputSizeInfo( p_dmo, 0, &i_size, &i_align ) )
        {
            msg_Err( p_dec, "GetOutputSizeInfo() failed" );
            goto error;
        }
        else
        {
            msg_Dbg( p_dec, "GetOutputSizeInfo(): bytes %i, align %i",
                     i_size, i_align );
            p_sys->i_min_output = i_size;
            p_sys->p_buffer = malloc( i_size );
            if( !p_sys->p_buffer ) goto error;
        }
    }
    /* Set output properties */
    p_dec->fmt_out.i_cat = p_dec->fmt_in.i_cat;
    if( p_dec->fmt_out.i_cat == AUDIO_ES )
        date_Init( &p_sys->end_date, p_dec->fmt_in.audio.i_rate, 1 );
    else
        date_Init( &p_sys->end_date, 25 /* FIXME */, 1 );
    free( p_vih );
    free( p_wf );
    vlc_mutex_lock( &p_sys->lock );
    p_sys->b_ready =
    p_sys->b_works = true;
    vlc_cond_signal( &p_sys->wait_output );
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
 error:
    if( p_dmo ) p_dmo->vt->Release( (IUnknown *)p_dmo );
    if( hmsdmo_dll ) FreeLibrary( hmsdmo_dll );
    /* Uninitialize OLE/COM */
    CoUninitialize();
    free( p_vih );
    free( p_wf );
    vlc_mutex_lock( &p_sys->lock );
    p_sys->b_ready = true;
    vlc_cond_signal( &p_sys->wait_output );
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_EGENERIC;
}
