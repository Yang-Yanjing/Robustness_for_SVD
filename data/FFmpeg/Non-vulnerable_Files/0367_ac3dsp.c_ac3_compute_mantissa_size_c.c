static int ac3_compute_mantissa_size_c(uint16_t mant_cnt[6][16])
{
    int blk, bap;
    int bits = 0;
    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        
        bits += (mant_cnt[blk][1] / 3) * 5;
        
        
        bits += ((mant_cnt[blk][2] / 3) + (mant_cnt[blk][4] >> 1)) * 7;
        
        bits += mant_cnt[blk][3] * 3;
        
        for (bap = 5; bap < 16; bap++)
            bits += mant_cnt[blk][bap] * ff_ac3_bap_bits[bap];
    }
    return bits;
}
