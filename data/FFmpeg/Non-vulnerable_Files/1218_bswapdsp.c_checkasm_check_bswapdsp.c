void checkasm_check_bswapdsp(void)
{
    LOCAL_ALIGNED_16(uint8_t, src0, [BUF_SIZE]);
    LOCAL_ALIGNED_16(uint8_t, src1, [BUF_SIZE]);
    LOCAL_ALIGNED_16(uint8_t, dst0, [BUF_SIZE]);
    LOCAL_ALIGNED_16(uint8_t, dst1, [BUF_SIZE]);
    BswapDSPContext h;
    ff_bswapdsp_init(&h);
    if (check_func(h.bswap_buf, "bswap_buf"))
        check_bswap(uint32_t);
    if (check_func(h.bswap16_buf, "bswap16_buf"))
        check_bswap(uint16_t);
    report("bswap");
}
