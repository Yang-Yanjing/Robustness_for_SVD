 *****************************************************************************/
static int EncoderSetVideoType( encoder_t *p_enc, IMediaObject *p_dmo )
{
    int i, i_selected, i_err;
    DMO_MEDIA_TYPE dmo_type;
    VIDEOINFOHEADER vih, *p_vih;
    VLC_BITMAPINFOHEADER *p_bih;
    /* FIXME */
    p_enc->fmt_in.video.i_bits_per_pixel =
        p_enc->fmt_out.video.i_bits_per_pixel = 12;
    /* Enumerate input format (for debug output) */
    i = 0;
    while( !p_dmo->vt->GetInputType( p_dmo, 0, i++, &dmo_type ) )
    {
        p_vih = (VIDEOINFOHEADER *)dmo_type.pbFormat;
        msg_Dbg( p_enc, "available input chroma: %4.4s",
                 (char *)&dmo_type.subtype.Data1 );
        if( !memcmp( &dmo_type.subtype, &MEDIASUBTYPE_RGB565, 16 ) )
            msg_Dbg( p_enc, "-> MEDIASUBTYPE_RGB565" );
        if( !memcmp( &dmo_type.subtype, &MEDIASUBTYPE_RGB24, 16 ) )
            msg_Dbg( p_enc, "-> MEDIASUBTYPE_RGB24" );
        DMOFreeMediaType( &dmo_type );
    }
    /* Setup input format */
    memset( &dmo_type, 0, sizeof(dmo_type) );
    memset( &vih, 0, sizeof(VIDEOINFOHEADER) );
    p_bih = &vih.bmiHeader;
    p_bih->biCompression = VLC_CODEC_I420;
    p_bih->biWidth = p_enc->fmt_in.video.i_visible_width;
    p_bih->biHeight = p_enc->fmt_in.video.i_visible_height;
    p_bih->biBitCount = p_enc->fmt_in.video.i_bits_per_pixel;
    p_bih->biSizeImage = p_enc->fmt_in.video.i_visible_width *
        p_enc->fmt_in.video.i_visible_height * p_enc->fmt_in.video.i_bits_per_pixel /8;
    p_bih->biPlanes = 3;
    p_bih->biSize = sizeof(VLC_BITMAPINFOHEADER);
    vih.rcSource.left = vih.rcSource.top = 0;
    vih.rcSource.right = p_enc->fmt_in.video.i_visible_width;
    vih.rcSource.bottom = p_enc->fmt_in.video.i_visible_height;
    vih.rcTarget = vih.rcSource;
    vih.AvgTimePerFrame = INT64_C(10000000) / 25; //FIXME
    dmo_type.majortype = MEDIATYPE_Video;
    //dmo_type.subtype = MEDIASUBTYPE_RGB24;
    dmo_type.subtype = MEDIASUBTYPE_I420;
    //dmo_type.subtype.Data1 = p_bih->biCompression;
    dmo_type.formattype = FORMAT_VideoInfo;
    dmo_type.bFixedSizeSamples = TRUE;
    dmo_type.bTemporalCompression = FALSE;
    dmo_type.lSampleSize = p_bih->biSizeImage;
    dmo_type.cbFormat = sizeof(VIDEOINFOHEADER);
    dmo_type.pbFormat = (char *)&vih;
    if( ( i_err = p_dmo->vt->SetInputType( p_dmo, 0, &dmo_type, 0 ) ) )
    {
        msg_Err( p_enc, "can't set DMO input type: %x", i_err );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_enc, "successfully set input type" );
    /* Setup output format */
    memset( &dmo_type, 0, sizeof(dmo_type) );
    dmo_type.pUnk = 0;
    /* Enumerate output types */
    i = 0, i_selected = -1;
    while( !p_dmo->vt->GetOutputType( p_dmo, 0, i++, &dmo_type ) )
    {
        p_vih = (VIDEOINFOHEADER *)dmo_type.pbFormat;
        msg_Dbg( p_enc, "available output codec: %4.4s",
                 (char *)&dmo_type.subtype.Data1 );
        if( p_vih->bmiHeader.biCompression == p_enc->fmt_out.i_codec )
            i_selected = i - 1;
        DMOFreeMediaType( &dmo_type );
    }
    if( i_selected < 0 )
    {
        msg_Err( p_enc, "couldn't find codec: %4.4s",
                 (char *)&p_enc->fmt_out.i_codec );
        return VLC_EGENERIC;
    }
    p_dmo->vt->GetOutputType( p_dmo, 0, i_selected, &dmo_type );
    ((VIDEOINFOHEADER *)dmo_type.pbFormat)->dwBitRate =
        p_enc->fmt_out.i_bitrate;
    /* Get the private data for the codec */
    while( 1 )
    {
        IWMCodecPrivateData *p_privdata;
        VIDEOINFOHEADER *p_vih;
        uint8_t *p_data = 0;
        uint32_t i_data = 0, i_vih;
        i_err = p_dmo->vt->QueryInterface( (IUnknown *)p_dmo,
                                           &IID_IWMCodecPrivateData,
                                           (void**)&p_privdata );
        if( i_err ) break;
        i_err = p_privdata->vt->SetPartialOutputType( p_privdata, &dmo_type );
        if( i_err )
        {
            msg_Err( p_enc, "SetPartialOutputType() failed" );
            p_privdata->vt->Release( (IUnknown *)p_privdata );
            break;
        }
        i_err = p_privdata->vt->GetPrivateData( p_privdata, NULL, &i_data );
        if( i_err )
        {
            msg_Err( p_enc, "GetPrivateData() failed" );
            p_privdata->vt->Release( (IUnknown *)p_privdata );
            break;
        }
        p_data = malloc( i_data );
        i_err = p_privdata->vt->GetPrivateData( p_privdata, p_data, &i_data );
        /* Update the media type with the private data */
        i_vih = dmo_type.cbFormat + i_data;
        p_vih = CoTaskMemAlloc( i_vih );
        memcpy( p_vih, dmo_type.pbFormat, dmo_type.cbFormat );
        memcpy( ((uint8_t *)p_vih) + dmo_type.cbFormat, p_data, i_data );
        DMOFreeMediaType( &dmo_type );
        dmo_type.pbFormat = (char*)p_vih;
        dmo_type.cbFormat = i_vih;
        msg_Dbg( p_enc, "found extra data: %i", i_data );
        p_enc->fmt_out.i_extra = i_data;
        p_enc->fmt_out.p_extra = p_data;
        break;
    }
    i_err = p_dmo->vt->SetOutputType( p_dmo, 0, &dmo_type, 0 );
    p_vih = (VIDEOINFOHEADER *)dmo_type.pbFormat;
    p_enc->fmt_in.i_codec = VLC_CODEC_I420;
    DMOFreeMediaType( &dmo_type );
    if( i_err )
    {
        msg_Err( p_enc, "can't set DMO output type for encoder: 0x%x", i_err );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_enc, "successfully set output type for encoder" );
    return VLC_SUCCESS;
}
