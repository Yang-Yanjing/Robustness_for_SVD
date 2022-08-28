static int mov_write_dvc1_structs(MOVTrack *track, uint8_t *buf)
{
    uint8_t *unescaped;
    const uint8_t *start, *next, *end = track->vos_data + track->vos_len;
    int unescaped_size, seq_found = 0;
    int level = 0, interlace = 0;
    int packet_seq   = track->vc1_info.packet_seq;
    int packet_entry = track->vc1_info.packet_entry;
    int slices       = track->vc1_info.slices;
    PutBitContext pbc;
    if (track->start_dts == AV_NOPTS_VALUE) {
        
        
        packet_seq = packet_entry = 1;
        av_log(NULL, AV_LOG_WARNING,
               "moov atom written before any packets, unable to write correct "
               "dvc1 atom. Set the delay_moov flag to fix this.\n");
    }
    unescaped = av_mallocz(track->vos_len + AV_INPUT_BUFFER_PADDING_SIZE);
    if (!unescaped)
        return AVERROR(ENOMEM);
    start = find_next_marker(track->vos_data, end);
    for (next = start; next < end; start = next) {
        GetBitContext gb;
        int size;
        next = find_next_marker(start + 4, end);
        size = next - start - 4;
        if (size <= 0)
            continue;
        unescaped_size = vc1_unescape_buffer(start + 4, size, unescaped);
        init_get_bits(&gb, unescaped, 8 * unescaped_size);
        if (AV_RB32(start) == VC1_CODE_SEQHDR) {
            int profile = get_bits(&gb, 2);
            if (profile != PROFILE_ADVANCED) {
                av_free(unescaped);
                return AVERROR(ENOSYS);
            }
            seq_found = 1;
            level = get_bits(&gb, 3);
            

            skip_bits_long(&gb, 2 + 3 + 5 + 1 + 2*12);
            skip_bits(&gb, 1); 
            interlace = get_bits1(&gb);
            skip_bits(&gb, 4); 
        }
    }
    if (!seq_found) {
        av_free(unescaped);
        return AVERROR(ENOSYS);
    }
    init_put_bits(&pbc, buf, 7);
    
    put_bits(&pbc, 4, 12); 
    put_bits(&pbc, 3, level);
    put_bits(&pbc, 1, 0); 
    
    put_bits(&pbc, 3, level);
    put_bits(&pbc, 1, 0); 
    put_bits(&pbc, 6, 0); 
    put_bits(&pbc, 1, !interlace); 
    put_bits(&pbc, 1, !packet_seq); 
    put_bits(&pbc, 1, !packet_entry); 
    put_bits(&pbc, 1, !slices); 
    put_bits(&pbc, 1, 0); 
    put_bits(&pbc, 1, 0); 
    
    if (track->st->avg_frame_rate.num > 0 && track->st->avg_frame_rate.den > 0)
        put_bits32(&pbc, track->st->avg_frame_rate.num / track->st->avg_frame_rate.den);
    else
        put_bits32(&pbc, 0xffffffff);
    flush_put_bits(&pbc);
    av_free(unescaped);
    return 0;
}
