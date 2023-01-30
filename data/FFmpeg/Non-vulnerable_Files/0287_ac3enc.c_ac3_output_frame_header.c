static void ac3_output_frame_header(AC3EncodeContext *s)
{
    AC3EncOptions *opt = &s->options;
    put_bits(&s->pb, 16, 0x0b77);   
    put_bits(&s->pb, 16, 0);        
    put_bits(&s->pb, 2,  s->bit_alloc.sr_code);
    put_bits(&s->pb, 6,  s->frame_size_code + (s->frame_size - s->frame_size_min) / 2);
    put_bits(&s->pb, 5,  s->bitstream_id);
    put_bits(&s->pb, 3,  s->bitstream_mode);
    put_bits(&s->pb, 3,  s->channel_mode);
    if ((s->channel_mode & 0x01) && s->channel_mode != AC3_CHMODE_MONO)
        put_bits(&s->pb, 2, s->center_mix_level);
    if (s->channel_mode & 0x04)
        put_bits(&s->pb, 2, s->surround_mix_level);
    if (s->channel_mode == AC3_CHMODE_STEREO)
        put_bits(&s->pb, 2, opt->dolby_surround_mode);
    put_bits(&s->pb, 1, s->lfe_on); 
    put_bits(&s->pb, 5, -opt->dialogue_level);
    put_bits(&s->pb, 1, 0);         
    put_bits(&s->pb, 1, 0);         
    put_bits(&s->pb, 1, opt->audio_production_info);
    if (opt->audio_production_info) {
        put_bits(&s->pb, 5, opt->mixing_level - 80);
        put_bits(&s->pb, 2, opt->room_type);
    }
    put_bits(&s->pb, 1, opt->copyright);
    put_bits(&s->pb, 1, opt->original);
    if (s->bitstream_id == 6) {
        
        put_bits(&s->pb, 1, opt->extended_bsi_1);
        if (opt->extended_bsi_1) {
            put_bits(&s->pb, 2, opt->preferred_stereo_downmix);
            put_bits(&s->pb, 3, s->ltrt_center_mix_level);
            put_bits(&s->pb, 3, s->ltrt_surround_mix_level);
            put_bits(&s->pb, 3, s->loro_center_mix_level);
            put_bits(&s->pb, 3, s->loro_surround_mix_level);
        }
        put_bits(&s->pb, 1, opt->extended_bsi_2);
        if (opt->extended_bsi_2) {
            put_bits(&s->pb, 2, opt->dolby_surround_ex_mode);
            put_bits(&s->pb, 2, opt->dolby_headphone_mode);
            put_bits(&s->pb, 1, opt->ad_converter_type);
            put_bits(&s->pb, 9, 0);     
        }
    } else {
    put_bits(&s->pb, 1, 0);         
    put_bits(&s->pb, 1, 0);         
    }
    put_bits(&s->pb, 1, 0);         
}
