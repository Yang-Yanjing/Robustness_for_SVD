static av_cold int validate_options(AC3EncodeContext *s)
{
    AVCodecContext *avctx = s->avctx;
    int i, ret, max_sr;
    
    if (!avctx->channel_layout) {
        av_log(avctx, AV_LOG_WARNING, "No channel layout specified. The "
                                      "encoder will guess the layout, but it "
                                      "might be incorrect.\n");
    }
    ret = set_channel_info(s, avctx->channels, &avctx->channel_layout);
    if (ret) {
        av_log(avctx, AV_LOG_ERROR, "invalid channel layout\n");
        return ret;
    }
    
    


    max_sr = s->eac3 ? 2 : 8;
    for (i = 0; i <= max_sr; i++) {
        if ((ff_ac3_sample_rate_tab[i % 3] >> (i / 3)) == avctx->sample_rate)
            break;
    }
    if (i > max_sr) {
        av_log(avctx, AV_LOG_ERROR, "invalid sample rate\n");
        return AVERROR(EINVAL);
    }
    s->sample_rate        = avctx->sample_rate;
    s->bit_alloc.sr_shift = i / 3;
    s->bit_alloc.sr_code  = i % 3;
    s->bitstream_id       = s->eac3 ? 16 : 8 + s->bit_alloc.sr_shift;
    
    if (!avctx->bit_rate) {
        switch (s->fbw_channels) {
        case 1: avctx->bit_rate =  96000; break;
        case 2: avctx->bit_rate = 192000; break;
        case 3: avctx->bit_rate = 320000; break;
        case 4: avctx->bit_rate = 384000; break;
        case 5: avctx->bit_rate = 448000; break;
        }
    }
    
    if (s->eac3) {
        int max_br, min_br, wpf, min_br_dist, min_br_code;
        int num_blks_code, num_blocks, frame_samples;
        
        


        for (num_blks_code = 3; num_blks_code >= 0; num_blks_code--) {
            num_blocks = ((int[]){ 1, 2, 3, 6 })[num_blks_code];
            frame_samples  = AC3_BLOCK_SIZE * num_blocks;
            max_br = 2048 * s->sample_rate / frame_samples * 16;
            min_br = ((s->sample_rate + (frame_samples-1)) / frame_samples) * 16;
            if (avctx->bit_rate <= max_br)
                break;
        }
        if (avctx->bit_rate < min_br || avctx->bit_rate > max_br) {
            av_log(avctx, AV_LOG_ERROR, "invalid bit rate. must be %d to %d "
                   "for this sample rate\n", min_br, max_br);
            return AVERROR(EINVAL);
        }
        s->num_blks_code = num_blks_code;
        s->num_blocks    = num_blocks;
        
        wpf = (avctx->bit_rate / 16) * frame_samples / s->sample_rate;
        av_assert1(wpf > 0 && wpf <= 2048);
        


        min_br_code = -1;
        min_br_dist = INT_MAX;
        for (i = 0; i < 19; i++) {
            int br_dist = abs(ff_ac3_bitrate_tab[i] * 1000 - avctx->bit_rate);
            if (br_dist < min_br_dist) {
                min_br_dist = br_dist;
                min_br_code = i;
            }
        }
        
        s->frame_size_code = min_br_code << 1;
        while (wpf > 1 && wpf * s->sample_rate / AC3_FRAME_SIZE * 16 > avctx->bit_rate)
            wpf--;
        s->frame_size_min = 2 * wpf;
    } else {
        int best_br = 0, best_code = 0, best_diff = INT_MAX;
        for (i = 0; i < 19; i++) {
            int br   = (ff_ac3_bitrate_tab[i] >> s->bit_alloc.sr_shift) * 1000;
            int diff = abs(br - avctx->bit_rate);
            if (diff < best_diff) {
                best_br   = br;
                best_code = i;
                best_diff = diff;
            }
            if (!best_diff)
                break;
        }
        avctx->bit_rate    = best_br;
        s->frame_size_code = best_code << 1;
        s->frame_size_min  = 2 * ff_ac3_frame_size_tab[s->frame_size_code][s->bit_alloc.sr_code];
        s->num_blks_code   = 0x3;
        s->num_blocks      = 6;
    }
    s->bit_rate   = avctx->bit_rate;
    s->frame_size = s->frame_size_min;
    
    if (avctx->cutoff < 0) {
        av_log(avctx, AV_LOG_ERROR, "invalid cutoff frequency\n");
        return AVERROR(EINVAL);
    }
    s->cutoff = avctx->cutoff;
    if (s->cutoff > (s->sample_rate >> 1))
        s->cutoff = s->sample_rate >> 1;
    ret = ff_ac3_validate_metadata(s);
    if (ret)
        return ret;
    s->rematrixing_enabled = s->options.stereo_rematrixing &&
                             (s->channel_mode == AC3_CHMODE_STEREO);
    s->cpl_enabled = s->options.channel_coupling &&
                     s->channel_mode >= AC3_CHMODE_STEREO;
    return 0;
}
