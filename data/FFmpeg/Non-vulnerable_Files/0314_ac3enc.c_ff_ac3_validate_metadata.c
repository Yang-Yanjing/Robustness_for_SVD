int ff_ac3_validate_metadata(AC3EncodeContext *s)
{
    AVCodecContext *avctx = s->avctx;
    AC3EncOptions *opt = &s->options;
    opt->audio_production_info = 0;
    opt->extended_bsi_1        = 0;
    opt->extended_bsi_2        = 0;
    opt->eac3_mixing_metadata  = 0;
    opt->eac3_info_metadata    = 0;
    
    if (s->channel_mode > AC3_CHMODE_STEREO && opt->preferred_stereo_downmix != AC3ENC_OPT_NONE) {
        opt->extended_bsi_1       = 1;
        opt->eac3_mixing_metadata = 1;
    }
    if (s->has_center &&
        (opt->ltrt_center_mix_level >= 0 || opt->loro_center_mix_level >= 0)) {
        opt->extended_bsi_1       = 1;
        opt->eac3_mixing_metadata = 1;
    }
    if (s->has_surround &&
        (opt->ltrt_surround_mix_level >= 0 || opt->loro_surround_mix_level >= 0)) {
        opt->extended_bsi_1       = 1;
        opt->eac3_mixing_metadata = 1;
    }
    if (s->eac3) {
        
        if (avctx->audio_service_type != AV_AUDIO_SERVICE_TYPE_MAIN)
            opt->eac3_info_metadata = 1;
        if (opt->copyright != AC3ENC_OPT_NONE || opt->original != AC3ENC_OPT_NONE)
            opt->eac3_info_metadata = 1;
        if (s->channel_mode == AC3_CHMODE_STEREO &&
            (opt->dolby_headphone_mode != AC3ENC_OPT_NONE || opt->dolby_surround_mode != AC3ENC_OPT_NONE))
            opt->eac3_info_metadata = 1;
        if (s->channel_mode >= AC3_CHMODE_2F2R && opt->dolby_surround_ex_mode != AC3ENC_OPT_NONE)
            opt->eac3_info_metadata = 1;
        if (opt->mixing_level != AC3ENC_OPT_NONE || opt->room_type != AC3ENC_OPT_NONE ||
            opt->ad_converter_type != AC3ENC_OPT_NONE) {
            opt->audio_production_info = 1;
            opt->eac3_info_metadata    = 1;
        }
    } else {
        
        if (opt->mixing_level != AC3ENC_OPT_NONE || opt->room_type != AC3ENC_OPT_NONE)
            opt->audio_production_info = 1;
        
        if (s->channel_mode >= AC3_CHMODE_2F2R && opt->dolby_surround_ex_mode != AC3ENC_OPT_NONE)
            opt->extended_bsi_2 = 1;
        if (s->channel_mode == AC3_CHMODE_STEREO && opt->dolby_headphone_mode != AC3ENC_OPT_NONE)
            opt->extended_bsi_2 = 1;
        if (opt->ad_converter_type != AC3ENC_OPT_NONE)
            opt->extended_bsi_2 = 1;
    }
    
    if (!s->eac3) {
        if (s->has_center) {
            validate_mix_level(avctx, "center_mix_level", &opt->center_mix_level,
                            cmixlev_options, CMIXLEV_NUM_OPTIONS, 1, 0,
                            &s->center_mix_level);
        }
        if (s->has_surround) {
            validate_mix_level(avctx, "surround_mix_level", &opt->surround_mix_level,
                            surmixlev_options, SURMIXLEV_NUM_OPTIONS, 1, 0,
                            &s->surround_mix_level);
        }
    }
    
    if (opt->extended_bsi_1 || opt->eac3_mixing_metadata) {
        
        if (opt->preferred_stereo_downmix == AC3ENC_OPT_NONE)
            opt->preferred_stereo_downmix = AC3ENC_OPT_NOT_INDICATED;
        if (!s->eac3 || s->has_center) {
            
            validate_mix_level(avctx, "ltrt_center_mix_level",
                               &opt->ltrt_center_mix_level, extmixlev_options,
                               EXTMIXLEV_NUM_OPTIONS, 5, 0,
                               &s->ltrt_center_mix_level);
            
            validate_mix_level(avctx, "loro_center_mix_level",
                               &opt->loro_center_mix_level, extmixlev_options,
                               EXTMIXLEV_NUM_OPTIONS, 5, 0,
                               &s->loro_center_mix_level);
        }
        if (!s->eac3 || s->has_surround) {
            
            validate_mix_level(avctx, "ltrt_surround_mix_level",
                               &opt->ltrt_surround_mix_level, extmixlev_options,
                               EXTMIXLEV_NUM_OPTIONS, 6, 3,
                               &s->ltrt_surround_mix_level);
            
            validate_mix_level(avctx, "loro_surround_mix_level",
                               &opt->loro_surround_mix_level, extmixlev_options,
                               EXTMIXLEV_NUM_OPTIONS, 6, 3,
                               &s->loro_surround_mix_level);
        }
    }
    
    if ((avctx->audio_service_type == AV_AUDIO_SERVICE_TYPE_KARAOKE &&
         avctx->channels == 1) ||
        ((avctx->audio_service_type == AV_AUDIO_SERVICE_TYPE_COMMENTARY ||
          avctx->audio_service_type == AV_AUDIO_SERVICE_TYPE_EMERGENCY  ||
          avctx->audio_service_type == AV_AUDIO_SERVICE_TYPE_VOICE_OVER)
         && avctx->channels > 1)) {
        av_log(avctx, AV_LOG_ERROR, "invalid audio service type for the "
                                    "specified number of channels\n");
        return AVERROR(EINVAL);
    }
    
    if (opt->extended_bsi_2 || opt->eac3_info_metadata) {
        
        if (opt->dolby_headphone_mode == AC3ENC_OPT_NONE)
            opt->dolby_headphone_mode = AC3ENC_OPT_NOT_INDICATED;
        
        if (opt->dolby_surround_ex_mode == AC3ENC_OPT_NONE)
            opt->dolby_surround_ex_mode = AC3ENC_OPT_NOT_INDICATED;
        
        if (opt->ad_converter_type == AC3ENC_OPT_NONE)
            opt->ad_converter_type = AC3ENC_OPT_ADCONV_STANDARD;
    }
    
    if (!s->eac3 || opt->eac3_info_metadata) {
        
        if (opt->copyright == AC3ENC_OPT_NONE)
            opt->copyright = AC3ENC_OPT_OFF;
        
        if (opt->original == AC3ENC_OPT_NONE)
            opt->original = AC3ENC_OPT_ON;
    }
    
    if (!s->eac3 || opt->eac3_info_metadata) {
        if (opt->dolby_surround_mode == AC3ENC_OPT_NONE)
            opt->dolby_surround_mode = AC3ENC_OPT_NOT_INDICATED;
    }
    
    if (opt->audio_production_info) {
        if (opt->mixing_level == AC3ENC_OPT_NONE) {
            av_log(avctx, AV_LOG_ERROR, "mixing_level must be set if "
                   "room_type is set\n");
            return AVERROR(EINVAL);
        }
        if (opt->mixing_level < 80) {
            av_log(avctx, AV_LOG_ERROR, "invalid mixing level. must be between "
                   "80dB and 111dB\n");
            return AVERROR(EINVAL);
        }
        
        if (opt->room_type == AC3ENC_OPT_NONE)
            opt->room_type = AC3ENC_OPT_NOT_INDICATED;
    }
    
    if (!s->eac3 && (opt->extended_bsi_1 || opt->extended_bsi_2)) {
        if (s->bitstream_id > 8 && s->bitstream_id < 11) {
            static int warn_once = 1;
            if (warn_once) {
                av_log(avctx, AV_LOG_WARNING, "alternate bitstream syntax is "
                       "not compatible with reduced samplerates. writing of "
                       "extended bitstream information will be disabled.\n");
                warn_once = 0;
            }
        } else {
            s->bitstream_id = 6;
        }
    }
    return 0;
}
