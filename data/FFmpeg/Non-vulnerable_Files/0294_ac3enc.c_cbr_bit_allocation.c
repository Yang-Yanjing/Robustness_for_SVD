static int cbr_bit_allocation(AC3EncodeContext *s)
{
    int ch;
    int bits_left;
    int snr_offset, snr_incr;
    bits_left = 8 * s->frame_size - (s->frame_bits + s->exponent_bits);
    if (bits_left < 0)
        return AVERROR(EINVAL);
    snr_offset = s->coarse_snr_offset << 4;
    

    if ((snr_offset | s->fine_snr_offset[1]) == 1023) {
        if (bit_alloc(s, 1023) <= bits_left)
            return 0;
    }
    while (snr_offset >= 0 &&
           bit_alloc(s, snr_offset) > bits_left) {
        snr_offset -= 64;
    }
    if (snr_offset < 0)
        return AVERROR(EINVAL);
    FFSWAP(uint8_t *, s->bap_buffer, s->bap1_buffer);
    for (snr_incr = 64; snr_incr > 0; snr_incr >>= 2) {
        while (snr_offset + snr_incr <= 1023 &&
               bit_alloc(s, snr_offset + snr_incr) <= bits_left) {
            snr_offset += snr_incr;
            FFSWAP(uint8_t *, s->bap_buffer, s->bap1_buffer);
        }
    }
    FFSWAP(uint8_t *, s->bap_buffer, s->bap1_buffer);
    reset_block_bap(s);
    s->coarse_snr_offset = snr_offset >> 4;
    for (ch = !s->cpl_on; ch <= s->channels; ch++)
        s->fine_snr_offset[ch] = snr_offset & 0xF;
    return 0;
}
