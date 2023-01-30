 *****************************************************************************/
static int EncoderSetAudioType( encoder_t *p_enc, IMediaObject *p_dmo )
{
    int i, i_selected, i_err;
    unsigned int i_last_byterate;
    uint16_t i_tag;
    DMO_MEDIA_TYPE dmo_type;
    WAVEFORMATEX *p_wf;
    /* Setup the format structure */
    fourcc_to_wf_tag( p_enc->fmt_out.i_codec, &i_tag );
    if( i_tag == 0 ) return VLC_EGENERIC;
    p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
    p_enc->fmt_in.audio.i_bitspersample = 16;
    /* We first need to choose an output type from the predefined
     * list of choices (we cycle through the list to select the best match) */
    i = 0; i_selected = -1; i_last_byterate = 0;
    while( !p_dmo->vt->GetOutputType( p_dmo, 0, i++, &dmo_type ) )
    {
        p_wf = (WAVEFORMATEX *)dmo_type.pbFormat;
        msg_Dbg( p_enc, "available format :%i, sample rate: %i, channels: %i, "
                 "bits per sample: %i, bitrate: %i, blockalign: %i",
                 (int) p_wf->wFormatTag, (int)p_wf->nSamplesPerSec,
                 (int)p_wf->nChannels, (int)p_wf->wBitsPerSample,
                 (int)p_wf->nAvgBytesPerSec * 8, (int)p_wf->nBlockAlign );
        if( p_wf->wFormatTag == i_tag &&
            p_wf->nSamplesPerSec == p_enc->fmt_in.audio.i_rate &&
            p_wf->nChannels == p_enc->fmt_in.audio.i_channels &&
            p_wf->wBitsPerSample == p_enc->fmt_in.audio.i_bitspersample )
        {
            if( p_wf->nAvgBytesPerSec <
                p_enc->fmt_out.i_bitrate * 110 / 800 /* + 10% */ &&
                p_wf->nAvgBytesPerSec > i_last_byterate )
            {
                i_selected = i - 1;
                i_last_byterate = p_wf->nAvgBytesPerSec;
                msg_Dbg( p_enc, "selected entry %i (bitrate: %i)",
                         i_selected, p_wf->nAvgBytesPerSec * 8 );
            }
        }
        DMOFreeMediaType( &dmo_type );
    }
    if( i_selected < 0 )
    {
        msg_Err( p_enc, "couldn't find a matching output" );
        return VLC_EGENERIC;
    }
    p_dmo->vt->GetOutputType( p_dmo, 0, i_selected, &dmo_type );
    p_wf = (WAVEFORMATEX *)dmo_type.pbFormat;
    msg_Dbg( p_enc, "selected format: %i, sample rate:%i, "
             "channels: %i, bits per sample: %i, bitrate: %i, blockalign: %i",
             (int)p_wf->wFormatTag, (int)p_wf->nSamplesPerSec,
             (int)p_wf->nChannels, (int)p_wf->wBitsPerSample,
             (int)p_wf->nAvgBytesPerSec * 8, (int)p_wf->nBlockAlign );
    p_enc->fmt_out.audio.i_rate = p_wf->nSamplesPerSec;
    p_enc->fmt_out.audio.i_channels = p_wf->nChannels;
    p_enc->fmt_out.audio.i_bitspersample = p_wf->wBitsPerSample;
    p_enc->fmt_out.audio.i_blockalign = p_wf->nBlockAlign;
    p_enc->fmt_out.i_bitrate = p_wf->nAvgBytesPerSec * 8;
    if( p_wf->cbSize )
    {
        msg_Dbg( p_enc, "found cbSize: %i", p_wf->cbSize );
        p_enc->fmt_out.i_extra = p_wf->cbSize;
        p_enc->fmt_out.p_extra = malloc( p_enc->fmt_out.i_extra );
        if( p_enc->fmt_out.p_extra == NULL)
            return VLC_EGENERIC;
        memcpy( p_enc->fmt_out.p_extra, &p_wf[1], p_enc->fmt_out.i_extra );
    }
    i_err = p_dmo->vt->SetOutputType( p_dmo, 0, &dmo_type, 0 );
    DMOFreeMediaType( &dmo_type );
    if( i_err )
    {
        msg_Err( p_enc, "can't set DMO output type: %i", i_err );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_enc, "successfully set output type" );
    /* Setup the input type */
    i = 0; i_selected = -1;
    while( !p_dmo->vt->GetInputType( p_dmo, 0, i++, &dmo_type ) )
    {
        p_wf = (WAVEFORMATEX *)dmo_type.pbFormat;
        msg_Dbg( p_enc, "available format :%i, sample rate: %i, channels: %i, "
                 "bits per sample: %i, bitrate: %i, blockalign: %i",
                 (int) p_wf->wFormatTag, (int)p_wf->nSamplesPerSec,
                 (int)p_wf->nChannels, (int)p_wf->wBitsPerSample,
                 (int)p_wf->nAvgBytesPerSec * 8, (int)p_wf->nBlockAlign );
        if( p_wf->wFormatTag == WAVE_FORMAT_PCM &&
            p_wf->nSamplesPerSec == p_enc->fmt_in.audio.i_rate &&
            p_wf->nChannels == p_enc->fmt_in.audio.i_channels &&
            p_wf->wBitsPerSample == p_enc->fmt_in.audio.i_bitspersample )
        {
            i_selected = i - 1;
        }
        DMOFreeMediaType( &dmo_type );
    }
    if( i_selected < 0 )
    {
        msg_Err( p_enc, "couldn't find a matching input" );
        return VLC_EGENERIC;
    }
    p_dmo->vt->GetInputType( p_dmo, 0, i_selected, &dmo_type );
    i_err = p_dmo->vt->SetInputType( p_dmo, 0, &dmo_type, 0 );
    DMOFreeMediaType( &dmo_type );
    if( i_err )
    {
        msg_Err( p_enc, "can't set DMO input type: 0x%x", i_err );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_enc, "successfully set input type" );
    return VLC_SUCCESS;
}
