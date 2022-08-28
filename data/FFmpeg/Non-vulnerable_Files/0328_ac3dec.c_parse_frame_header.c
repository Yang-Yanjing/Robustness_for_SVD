static int parse_frame_header(AC3DecodeContext *s)
{
    AC3HeaderInfo hdr, *phdr=&hdr;
    int err;
    err = avpriv_ac3_parse_header2(&s->gbc, &phdr);
    if (err)
        return err;
    
    s->bit_alloc_params.sr_code     = hdr.sr_code;
    s->bitstream_id                 = hdr.bitstream_id;
    s->bitstream_mode               = hdr.bitstream_mode;
    s->channel_mode                 = hdr.channel_mode;
    s->lfe_on                       = hdr.lfe_on;
    s->bit_alloc_params.sr_shift    = hdr.sr_shift;
    s->sample_rate                  = hdr.sample_rate;
    s->bit_rate                     = hdr.bit_rate;
    s->channels                     = hdr.channels;
    s->fbw_channels                 = s->channels - s->lfe_on;
    s->lfe_ch                       = s->fbw_channels + 1;
    s->frame_size                   = hdr.frame_size;
    s->preferred_downmix            = AC3_DMIXMOD_NOTINDICATED;
    s->center_mix_level             = hdr.center_mix_level;
    s->center_mix_level_ltrt        = 4; 
    s->surround_mix_level           = hdr.surround_mix_level;
    s->surround_mix_level_ltrt      = 4; 
    s->lfe_mix_level_exists         = 0;
    s->num_blocks                   = hdr.num_blocks;
    s->frame_type                   = hdr.frame_type;
    s->substreamid                  = hdr.substreamid;
    s->dolby_surround_mode          = hdr.dolby_surround_mode;
    s->dolby_surround_ex_mode       = AC3_DSUREXMOD_NOTINDICATED;
    s->dolby_headphone_mode         = AC3_DHEADPHONMOD_NOTINDICATED;
    if (s->lfe_on) {
        s->start_freq[s->lfe_ch]     = 0;
        s->end_freq[s->lfe_ch]       = 7;
        s->num_exp_groups[s->lfe_ch] = 2;
        s->channel_in_cpl[s->lfe_ch] = 0;
    }
    if (s->bitstream_id <= 10) {
        s->eac3                  = 0;
        s->snr_offset_strategy   = 2;
        s->block_switch_syntax   = 1;
        s->dither_flag_syntax    = 1;
        s->bit_allocation_syntax = 1;
        s->fast_gain_syntax      = 0;
        s->first_cpl_leak        = 0;
        s->dba_syntax            = 1;
        s->skip_syntax           = 1;
        memset(s->channel_uses_aht, 0, sizeof(s->channel_uses_aht));
        return ac3_parse_header(s);
    } else if (CONFIG_EAC3_DECODER) {
        s->eac3 = 1;
        return ff_eac3_parse_header(s);
    } else {
        av_log(s->avctx, AV_LOG_ERROR, "E-AC-3 support not compiled in\n");
        return AVERROR(ENOSYS);
    }
}
