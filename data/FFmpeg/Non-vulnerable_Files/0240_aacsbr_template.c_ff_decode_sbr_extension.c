int AAC_RENAME(ff_decode_sbr_extension)(AACContext *ac, SpectralBandReplication *sbr,
                            GetBitContext *gb_host, int crc, int cnt, int id_aac)
{
    unsigned int num_sbr_bits = 0, num_align_bits;
    unsigned bytes_read;
    GetBitContext gbc = *gb_host, *gb = &gbc;
    skip_bits_long(gb_host, cnt*8 - 4);
    sbr->reset = 0;
    if (!sbr->sample_rate)
        sbr->sample_rate = 2 * ac->oc[1].m4ac.sample_rate; 
    if (!ac->oc[1].m4ac.ext_sample_rate)
        ac->oc[1].m4ac.ext_sample_rate = 2 * ac->oc[1].m4ac.sample_rate;
    if (crc) {
        skip_bits(gb, 10); 
        num_sbr_bits += 10;
    }
    
    sbr->kx[0] = sbr->kx[1];
    sbr->m[0] = sbr->m[1];
    sbr->kx_and_m_pushed = 1;
    num_sbr_bits++;
    if (get_bits1(gb)) 
        num_sbr_bits += read_sbr_header(sbr, gb);
    if (sbr->reset)
        sbr_reset(ac, sbr);
    if (sbr->start)
        num_sbr_bits  += read_sbr_data(ac, sbr, gb, id_aac);
    num_align_bits = ((cnt << 3) - 4 - num_sbr_bits) & 7;
    bytes_read = ((num_sbr_bits + num_align_bits + 4) >> 3);
    if (bytes_read > cnt) {
        av_log(ac->avctx, AV_LOG_ERROR,
               "Expected to read %d SBR bytes actually read %d.\n", cnt, bytes_read);
    }
    return cnt;
}
