static int mov_write_audio_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    int version = 0;
    uint32_t tag = track->tag;
    if (track->mode == MODE_MOV) {
        if (track->timescale > UINT16_MAX) {
            if (mov_get_lpcm_flags(track->enc->codec_id))
                tag = AV_RL32("lpcm");
            version = 2;
        } else if (track->audio_vbr || mov_pcm_le_gt16(track->enc->codec_id) ||
                   mov_pcm_be_gt16(track->enc->codec_id) ||
                   track->enc->codec_id == AV_CODEC_ID_ADPCM_MS ||
                   track->enc->codec_id == AV_CODEC_ID_ADPCM_IMA_WAV ||
                   track->enc->codec_id == AV_CODEC_ID_QDM2) {
            version = 1;
        }
    }
    avio_wb32(pb, 0); 
    avio_wl32(pb, tag); 
    avio_wb32(pb, 0); 
    avio_wb16(pb, 0); 
    avio_wb16(pb, 1); 
    
    avio_wb16(pb, version); 
    avio_wb16(pb, 0); 
    avio_wb32(pb, 0); 
    if (version == 2) {
        avio_wb16(pb, 3);
        avio_wb16(pb, 16);
        avio_wb16(pb, 0xfffe);
        avio_wb16(pb, 0);
        avio_wb32(pb, 0x00010000);
        avio_wb32(pb, 72);
        avio_wb64(pb, av_double2int(track->enc->sample_rate));
        avio_wb32(pb, track->enc->channels);
        avio_wb32(pb, 0x7F000000);
        avio_wb32(pb, av_get_bits_per_sample(track->enc->codec_id));
        avio_wb32(pb, mov_get_lpcm_flags(track->enc->codec_id));
        avio_wb32(pb, track->sample_size);
        avio_wb32(pb, get_samples_per_packet(track));
    } else {
        if (track->mode == MODE_MOV) {
            avio_wb16(pb, track->enc->channels);
            if (track->enc->codec_id == AV_CODEC_ID_PCM_U8 ||
                track->enc->codec_id == AV_CODEC_ID_PCM_S8)
                avio_wb16(pb, 8); 
            else if (track->enc->codec_id == AV_CODEC_ID_ADPCM_G726)
                avio_wb16(pb, track->enc->bits_per_coded_sample);
            else
                avio_wb16(pb, 16);
            avio_wb16(pb, track->audio_vbr ? -2 : 0); 
        } else { 
            avio_wb16(pb, 2);
            avio_wb16(pb, 16);
            avio_wb16(pb, 0);
        }
        avio_wb16(pb, 0); 
        avio_wb16(pb, track->enc->sample_rate <= UINT16_MAX ?
                      track->enc->sample_rate : 0);
        avio_wb16(pb, 0); 
    }
    if (version == 1) { 
        if (mov_pcm_le_gt16(track->enc->codec_id) ||
            mov_pcm_be_gt16(track->enc->codec_id))
            avio_wb32(pb, 1); 
        else
            avio_wb32(pb, track->enc->frame_size); 
        avio_wb32(pb, track->sample_size / track->enc->channels); 
        avio_wb32(pb, track->sample_size); 
        avio_wb32(pb, 2); 
    }
    if (track->mode == MODE_MOV &&
        (track->enc->codec_id == AV_CODEC_ID_AAC           ||
         track->enc->codec_id == AV_CODEC_ID_AC3           ||
         track->enc->codec_id == AV_CODEC_ID_EAC3          ||
         track->enc->codec_id == AV_CODEC_ID_AMR_NB        ||
         track->enc->codec_id == AV_CODEC_ID_ALAC          ||
         track->enc->codec_id == AV_CODEC_ID_ADPCM_MS      ||
         track->enc->codec_id == AV_CODEC_ID_ADPCM_IMA_WAV ||
         track->enc->codec_id == AV_CODEC_ID_QDM2          ||
         (mov_pcm_le_gt16(track->enc->codec_id) && version==1) ||
         (mov_pcm_be_gt16(track->enc->codec_id) && version==1)))
        mov_write_wave_tag(pb, track);
    else if (track->tag == MKTAG('m','p','4','a'))
        mov_write_esds_tag(pb, track);
    else if (track->enc->codec_id == AV_CODEC_ID_AMR_NB)
        mov_write_amr_tag(pb, track);
    else if (track->enc->codec_id == AV_CODEC_ID_AC3)
        mov_write_ac3_tag(pb, track);
    else if (track->enc->codec_id == AV_CODEC_ID_EAC3)
        mov_write_eac3_tag(pb, track);
    else if (track->enc->codec_id == AV_CODEC_ID_ALAC)
        mov_write_extradata_tag(pb, track);
    else if (track->enc->codec_id == AV_CODEC_ID_WMAPRO)
        mov_write_wfex_tag(pb, track);
    else if (track->vos_len > 0)
        mov_write_glbl_tag(pb, track);
    if (track->mode == MODE_MOV && track->enc->codec_type == AVMEDIA_TYPE_AUDIO)
        mov_write_chan_tag(pb, track);
    return update_size(pb, pos);
}
