static int ac3_decode_frame(AVCodecContext * avctx, void *data,
                            int *got_frame_ptr, AVPacket *avpkt)
{
    AVFrame *frame     = data;
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    AC3DecodeContext *s = avctx->priv_data;
    int blk, ch, err, ret;
    const uint8_t *channel_map;
    const SHORTFLOAT *output[AC3_MAX_CHANNELS];
    enum AVMatrixEncoding matrix_encoding;
    AVDownmixInfo *downmix_info;
    

    if (buf_size >= 2 && AV_RB16(buf) == 0x770B) {
        
        int cnt = FFMIN(buf_size, AC3_FRAME_BUFFER_SIZE) >> 1;
        s->bdsp.bswap16_buf((uint16_t *) s->input_buffer,
                            (const uint16_t *) buf, cnt);
    } else
        memcpy(s->input_buffer, buf, FFMIN(buf_size, AC3_FRAME_BUFFER_SIZE));
    buf = s->input_buffer;
    
    if ((ret = init_get_bits8(&s->gbc, buf, buf_size)) < 0)
        return ret;
    
    err = parse_frame_header(s);
    if (err) {
        switch (err) {
        case AAC_AC3_PARSE_ERROR_SYNC:
            av_log(avctx, AV_LOG_ERROR, "frame sync error\n");
            return AVERROR_INVALIDDATA;
        case AAC_AC3_PARSE_ERROR_BSID:
            av_log(avctx, AV_LOG_ERROR, "invalid bitstream id\n");
            break;
        case AAC_AC3_PARSE_ERROR_SAMPLE_RATE:
            av_log(avctx, AV_LOG_ERROR, "invalid sample rate\n");
            break;
        case AAC_AC3_PARSE_ERROR_FRAME_SIZE:
            av_log(avctx, AV_LOG_ERROR, "invalid frame size\n");
            break;
        case AAC_AC3_PARSE_ERROR_FRAME_TYPE:
            
            
            if (s->frame_type == EAC3_FRAME_TYPE_DEPENDENT || s->substreamid) {
                av_log(avctx, AV_LOG_WARNING, "unsupported frame type : "
                       "skipping frame\n");
                *got_frame_ptr = 0;
                return buf_size;
            } else {
                av_log(avctx, AV_LOG_ERROR, "invalid frame type\n");
            }
            break;
        case AAC_AC3_PARSE_ERROR_CRC:
        case AAC_AC3_PARSE_ERROR_CHANNEL_CFG:
            break;
        default: 
            *got_frame_ptr = 0;
            return err;
        }
    } else {
        
        if (s->frame_size > buf_size) {
            av_log(avctx, AV_LOG_ERROR, "incomplete frame\n");
            err = AAC_AC3_PARSE_ERROR_FRAME_SIZE;
        } else if (avctx->err_recognition & (AV_EF_CRCCHECK|AV_EF_CAREFUL)) {
            
            if (av_crc(av_crc_get_table(AV_CRC_16_ANSI), 0, &buf[2],
                       s->frame_size - 2)) {
                av_log(avctx, AV_LOG_ERROR, "frame CRC mismatch\n");
                if (avctx->err_recognition & AV_EF_EXPLODE)
                    return AVERROR_INVALIDDATA;
                err = AAC_AC3_PARSE_ERROR_CRC;
            }
        }
    }
    
    if (!err) {
        avctx->sample_rate = s->sample_rate;
        avctx->bit_rate    = s->bit_rate;
    }
    
    if (!err || (s->channels && s->out_channels != s->channels)) {
        s->out_channels = s->channels;
        s->output_mode  = s->channel_mode;
        if (s->lfe_on)
            s->output_mode |= AC3_OUTPUT_LFEON;
        if (s->channels > 1 &&
            avctx->request_channel_layout == AV_CH_LAYOUT_MONO) {
            s->out_channels = 1;
            s->output_mode  = AC3_CHMODE_MONO;
        } else if (s->channels > 2 &&
                   avctx->request_channel_layout == AV_CH_LAYOUT_STEREO) {
            s->out_channels = 2;
            s->output_mode  = AC3_CHMODE_STEREO;
        }
        s->loro_center_mix_level   = gain_levels[s->  center_mix_level];
        s->loro_surround_mix_level = gain_levels[s->surround_mix_level];
        s->ltrt_center_mix_level   = LEVEL_MINUS_3DB;
        s->ltrt_surround_mix_level = LEVEL_MINUS_3DB;
        
        if (s->channels != s->out_channels && !((s->output_mode & AC3_OUTPUT_LFEON) &&
                s->fbw_channels == s->out_channels)) {
            set_downmix_coeffs(s);
        }
    } else if (!s->channels) {
        av_log(avctx, AV_LOG_ERROR, "unable to determine channel mode\n");
        return AVERROR_INVALIDDATA;
    }
    avctx->channels = s->out_channels;
    avctx->channel_layout = avpriv_ac3_channel_layout_tab[s->output_mode & ~AC3_OUTPUT_LFEON];
    if (s->output_mode & AC3_OUTPUT_LFEON)
        avctx->channel_layout |= AV_CH_LOW_FREQUENCY;
    
    avctx->audio_service_type = s->bitstream_mode;
    if (s->bitstream_mode == 0x7 && s->channels > 1)
        avctx->audio_service_type = AV_AUDIO_SERVICE_TYPE_KARAOKE;
    
    frame->nb_samples = s->num_blocks * AC3_BLOCK_SIZE;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;
    
    channel_map = ff_ac3_dec_channel_map[s->output_mode & ~AC3_OUTPUT_LFEON][s->lfe_on];
    for (ch = 0; ch < AC3_MAX_CHANNELS; ch++) {
        output[ch] = s->output[ch];
        s->outptr[ch] = s->output[ch];
    }
    for (ch = 0; ch < s->channels; ch++) {
        if (ch < s->out_channels)
            s->outptr[channel_map[ch]] = (SHORTFLOAT *)frame->data[ch];
    }
    for (blk = 0; blk < s->num_blocks; blk++) {
        if (!err && decode_audio_block(s, blk)) {
            av_log(avctx, AV_LOG_ERROR, "error decoding the audio block\n");
            err = 1;
        }
        if (err)
            for (ch = 0; ch < s->out_channels; ch++)
                memcpy(((SHORTFLOAT*)frame->data[ch]) + AC3_BLOCK_SIZE*blk, output[ch], AC3_BLOCK_SIZE*sizeof(SHORTFLOAT));
        for (ch = 0; ch < s->out_channels; ch++)
            output[ch] = s->outptr[channel_map[ch]];
        for (ch = 0; ch < s->out_channels; ch++) {
            if (!ch || channel_map[ch])
                s->outptr[channel_map[ch]] += AC3_BLOCK_SIZE;
        }
    }
    av_frame_set_decode_error_flags(frame, err ? FF_DECODE_ERROR_INVALID_BITSTREAM : 0);
    
    for (ch = 0; ch < s->out_channels; ch++)
        memcpy(s->output[ch], output[ch], AC3_BLOCK_SIZE*sizeof(SHORTFLOAT));
    





    matrix_encoding = AV_MATRIX_ENCODING_NONE;
    if (s->channel_mode == AC3_CHMODE_STEREO &&
        s->channel_mode == (s->output_mode & ~AC3_OUTPUT_LFEON)) {
        if (s->dolby_surround_mode == AC3_DSURMOD_ON)
            matrix_encoding = AV_MATRIX_ENCODING_DOLBY;
        else if (s->dolby_headphone_mode == AC3_DHEADPHONMOD_ON)
            matrix_encoding = AV_MATRIX_ENCODING_DOLBYHEADPHONE;
    } else if (s->channel_mode >= AC3_CHMODE_2F2R &&
               s->channel_mode == (s->output_mode & ~AC3_OUTPUT_LFEON)) {
        switch (s->dolby_surround_ex_mode) {
        case AC3_DSUREXMOD_ON: 
            matrix_encoding = AV_MATRIX_ENCODING_DOLBYEX;
            break;
        case AC3_DSUREXMOD_PLIIZ:
            matrix_encoding = AV_MATRIX_ENCODING_DPLIIZ;
            break;
        default: 
            break;
        }
    }
    if ((ret = ff_side_data_update_matrix_encoding(frame, matrix_encoding)) < 0)
        return ret;
    
    if ((downmix_info = av_downmix_info_update_side_data(frame))) {
        switch (s->preferred_downmix) {
        case AC3_DMIXMOD_LTRT:
            downmix_info->preferred_downmix_type = AV_DOWNMIX_TYPE_LTRT;
            break;
        case AC3_DMIXMOD_LORO:
            downmix_info->preferred_downmix_type = AV_DOWNMIX_TYPE_LORO;
            break;
        case AC3_DMIXMOD_DPLII:
            downmix_info->preferred_downmix_type = AV_DOWNMIX_TYPE_DPLII;
            break;
        default:
            downmix_info->preferred_downmix_type = AV_DOWNMIX_TYPE_UNKNOWN;
            break;
        }
        downmix_info->center_mix_level        = gain_levels[s->       center_mix_level];
        downmix_info->center_mix_level_ltrt   = gain_levels[s->  center_mix_level_ltrt];
        downmix_info->surround_mix_level      = gain_levels[s->     surround_mix_level];
        downmix_info->surround_mix_level_ltrt = gain_levels[s->surround_mix_level_ltrt];
        if (s->lfe_mix_level_exists)
            downmix_info->lfe_mix_level       = gain_levels_lfe[s->lfe_mix_level];
        else
            downmix_info->lfe_mix_level       = 0.0; 
    } else
        return AVERROR(ENOMEM);
    *got_frame_ptr = 1;
    return FFMIN(buf_size, s->frame_size);
}
