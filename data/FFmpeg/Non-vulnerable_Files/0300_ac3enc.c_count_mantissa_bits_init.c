static void count_mantissa_bits_init(uint16_t mant_cnt[AC3_MAX_BLOCKS][16])
{
    int blk;
    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        memset(mant_cnt[blk], 0, sizeof(mant_cnt[blk]));
        mant_cnt[blk][1] = mant_cnt[blk][2] = 2;
        mant_cnt[blk][4] = 1;
    }
}
