 *****************************************************************************/
static int OpenEncoder(vlc_object_t *p_this)
{
    encoder_t *p_enc = (encoder_t *)p_this;
    config_ChainParse(p_enc, ENC_CFG_PREFIX, ppsz_enc_options, p_enc->p_cfg);
    int i_aot;
    switch (p_enc->fmt_out.i_codec) {
    case VLC_CODEC_MP4A:
        i_aot = var_InheritInteger(p_enc, ENC_CFG_PREFIX "profile");
        break;
    case VLC_FOURCC('l', 'a', 'a', 'c'):
        i_aot = PROFILE_AAC_LC;
        break;
    case VLC_FOURCC('h', 'a', 'a', 'c'):
        i_aot = PROFILE_AAC_HE;
        break;
    case VLC_FOURCC('s', 'a', 'a', 'c'):
        i_aot = PROFILE_AAC_HE_v2;
        break;
    default:
        return VLC_EGENERIC;
    }
    if (p_enc->fmt_in.audio.i_channels != 2)
        if (i_aot == PROFILE_AAC_HE_v2 || i_aot == PROFILE_AAC_ELD) {
            msg_Err(p_enc, "Selected profile %d can only be used with stereo", i_aot);
            return VLC_EGENERIC;
        }
    uint16_t channel_config;
    CHANNEL_MODE mode;
    switch (p_enc->fmt_in.audio.i_channels) {
    case 1: mode = MODE_1; channel_config = AOUT_CHAN_CENTER; break;
    case 2: mode = MODE_2; channel_config = AOUT_CHANS_STEREO; break;
    case 3: mode = MODE_1_2; channel_config = AOUT_CHANS_3_0; break;
    case 4: mode = MODE_1_2_1; channel_config = AOUT_CHANS_4_CENTER_REAR; break;
    case 5: mode = MODE_1_2_2; channel_config = AOUT_CHANS_5_0; break;
    case 6: mode = MODE_1_2_2_1; channel_config = AOUT_CHANS_5_1; break;
    case 8: mode = MODE_1_2_2_2_1; channel_config = AOUT_CHANS_7_1; break;
    default:
        msg_Err(p_enc, "we do not support > 8 input channels, this input has %i",
                        p_enc->fmt_in.audio.i_channels);
        return VLC_EGENERIC;
    }
    p_enc->fmt_in.audio.i_physical_channels = channel_config;
    msg_Info(p_enc, "Initializing AAC Encoder, %i channels", p_enc->fmt_in.audio.i_channels);
    /* Allocate the memory needed to store the encoder's structure */
    encoder_sys_t *p_sys = (encoder_sys_t *)malloc(sizeof(encoder_sys_t));
    if (unlikely(!p_sys))
        return VLC_ENOMEM;
    p_enc->p_sys = p_sys;
    p_enc->fmt_in.i_codec = VLC_CODEC_S16N;
    p_enc->fmt_out.i_cat = AUDIO_ES;
    p_enc->fmt_out.i_codec = VLC_CODEC_MP4A;
    p_sys->i_pts_last = 0;
    AACENC_ERROR erraac;
    erraac = aacEncOpen(&p_sys->handle, 0, p_enc->fmt_in.audio.i_channels);
    if (erraac != AACENC_OK) {
        msg_Err(p_enc, "Unable to open encoder: %s", fdkaac_error(erraac));
        free(p_sys);
        return VLC_EGENERIC;
    }
#define SET_PARAM(P, V) do { \
        AACENC_ERROR err = aacEncoder_SetParam(p_sys->handle, AACENC_ ## P, V); \
        if (err != AACENC_OK) { \
            msg_Err(p_enc, "Couldn't set " #P " to value %d: %s", V, fdkaac_error(err)); \
            goto error; \
        } \
    } while(0)
    SET_PARAM(AOT, i_aot);
    bool b_eld_sbr = var_InheritBool(p_enc, ENC_CFG_PREFIX "sbr");
    if (i_aot == PROFILE_AAC_ELD && b_eld_sbr)
        SET_PARAM(SBR_MODE, 1);
    SET_PARAM(SAMPLERATE, p_enc->fmt_out.audio.i_rate);
    SET_PARAM(CHANNELMODE, mode);
    SET_PARAM(CHANNELORDER, CH_ORDER_WG4);
    int i_vbr = var_InheritInteger(p_enc, ENC_CFG_PREFIX "vbr");
    if (i_vbr != 0) {
        if ((i_aot == PROFILE_AAC_HE || i_aot == PROFILE_AAC_HE_v2) && i_vbr > 3) {
            msg_Warn(p_enc, "Maximum VBR quality for this profile is 3, setting vbr=3");
            i_vbr = 3;
        }
        SET_PARAM(BITRATEMODE, i_vbr);
    } else {
        int i_bitrate = p_enc->fmt_out.i_bitrate;
        if (i_bitrate == 0) {
            i_bitrate = 96 * p_enc->fmt_in.audio.i_channels * p_enc->fmt_out.audio.i_rate / 44;
            if (i_aot == PROFILE_AAC_HE || i_aot == PROFILE_AAC_HE_v2 || b_eld_sbr)
                i_bitrate /= 2;
            p_enc->fmt_out.i_bitrate = i_bitrate;
            msg_Info(p_enc, "Setting optimal bitrate of %i", i_bitrate);
        }
        SET_PARAM(BITRATE, i_bitrate);
    }
    SET_PARAM(TRANSMUX, 0);
    SET_PARAM(SIGNALING_MODE, (int)var_InheritInteger(p_enc, ENC_CFG_PREFIX "signaling"));
    SET_PARAM(AFTERBURNER, !!var_InheritBool(p_enc, ENC_CFG_PREFIX "afterburner"));
#undef SET_PARAM
    erraac = aacEncEncode(p_sys->handle, NULL, NULL, NULL, NULL);
    if (erraac != AACENC_OK) {
        msg_Err(p_enc, "Unable to initialize the encoder: %s", fdkaac_error(erraac));
        goto error;
    }
    AACENC_InfoStruct info = { 0 };
    erraac = aacEncInfo(p_sys->handle, &info);
    if (erraac != AACENC_OK) {
        msg_Err(p_enc, "Unable to get the encoder info: %s", fdkaac_error(erraac));
        goto error;
    }
    /* The maximum packet size is 6144 bits aka 768 bytes per channel. */
    p_sys->i_maxoutputsize = 768*p_enc->fmt_in.audio.i_channels;
    p_enc->fmt_in.audio.i_bitspersample = 16;
    p_sys->i_frame_size = info.frameLength;
    p_sys->i_encoderdelay = info.encoderDelay;
    p_enc->fmt_out.i_extra = info.confSize;
    if (p_enc->fmt_out.i_extra) {
        p_enc->fmt_out.p_extra = malloc(p_enc->fmt_out.i_extra);
        if (p_enc->fmt_out.p_extra == NULL) {
            msg_Err(p_enc, "Unable to allocate fmt_out.p_extra");
            goto error;
        }
        memcpy(p_enc->fmt_out.p_extra, info.confBuf, p_enc->fmt_out.i_extra);
    }
    p_enc->pf_encode_audio = EncodeAudio;
#ifndef NDEBUG
    // TODO: Add more debug info to this config printout
    msg_Dbg(p_enc, "fmt_out.p_extra = %i", p_enc->fmt_out.i_extra);
#endif
    return VLC_SUCCESS;
error:
    CloseEncoder(p_this);
    return VLC_EGENERIC;
}
