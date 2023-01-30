}
static int OpenEncoder(vlc_object_t *p_this)
{
    encoder_t *enc = (encoder_t *)p_this;
    if (enc->fmt_out.i_codec != VLC_CODEC_OPUS)
        return VLC_EGENERIC;
    encoder_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    int status = VLC_SUCCESS;
    sys->buffer = NULL;
    sys->enc = NULL;
    enc->pf_encode_audio = Encode;
    enc->fmt_in.i_codec = VLC_CODEC_FL32;
    enc->fmt_in.audio.i_rate = /* Only 48kHz */
    enc->fmt_out.audio.i_rate = 48000;
    enc->fmt_out.audio.i_channels = enc->fmt_in.audio.i_channels;
    OpusHeader header;
    if (opus_prepare_header(enc->fmt_out.audio.i_channels,
            enc->fmt_out.audio.i_rate,
            &header))
    {
        msg_Err(enc, "Failed to prepare header.");
        status = VLC_ENOMEM;
        goto error;
    }
    /* needed for max encoded size calculation */
    sys->nb_streams = header.nb_streams;
    int err;
    sys->enc =
        opus_multistream_surround_encoder_create(enc->fmt_in.audio.i_rate,
                enc->fmt_in.audio.i_channels, header.channel_mapping,
                &header.nb_streams, &header.nb_coupled, header.stream_map,
                OPUS_APPLICATION_AUDIO, &err);
    if (err != OPUS_OK)
    {
        msg_Err(enc, "Could not create encoder: error %d", err);
        sys->enc = NULL;
        status = VLC_EGENERIC;
        goto error;
    }
    /* TODO: vbr, fec */
    if( enc->fmt_out.i_bitrate )
        opus_multistream_encoder_ctl(sys->enc, OPUS_SET_BITRATE( enc->fmt_out.i_bitrate ));
    /* Buffer for incoming audio, since opus only accepts frame sizes that are
       multiples of 2.5ms */
    enc->p_sys = sys;
    sys->buffer = malloc(OPUS_FRAME_SIZE * header.channels * sizeof(float));
    if (!sys->buffer) {
        status = VLC_ENOMEM;
        goto error;
    }
    sys->i_nb_samples = 0;
    sys->i_samples_delay = 0;
    int ret = opus_multistream_encoder_ctl(enc->p_sys->enc,
            OPUS_GET_LOOKAHEAD(&sys->i_samples_delay));
    if (ret != OPUS_OK)
        msg_Err(enc, "Unable to get number of lookahead samples: %s\n",
                opus_strerror(ret));
    header.preskip = sys->i_samples_delay;
    /* Now that we have preskip, we can write the header to extradata */
    if (opus_write_header((uint8_t **) &enc->fmt_out.p_extra,
                          &enc->fmt_out.i_extra, &header, opus_get_version_string()))
    {
        msg_Err(enc, "Failed to write header.");
        status = VLC_ENOMEM;
        goto error;
    }
    if (sys->i_samples_delay > 0)
    {
        const unsigned padding_samples = sys->i_samples_delay *
            enc->fmt_out.audio.i_channels;
        sys->padding = block_Alloc(padding_samples * sizeof(float));
        if (!sys->padding) {
            status = VLC_ENOMEM;
            goto error;
        }
        sys->padding->i_nb_samples = sys->i_samples_delay;
        float *pad_ptr = (float *) sys->padding->p_buffer;
        memset(pad_ptr, 0, padding_samples * sizeof(float));
    }
    else
    {
        sys->padding = NULL;
    }
    return status;
error:
    if (sys->enc)
        opus_multistream_encoder_destroy(sys->enc);
    free(sys->buffer);
    free(sys);
    return status;
}
