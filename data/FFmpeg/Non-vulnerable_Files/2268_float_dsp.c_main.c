int main(int argc, char **argv)
{
    int ret = 0, seeded = 0;
    uint32_t seed;
    AVFloatDSPContext fdsp, cdsp;
    AVLFG lfg;
    LOCAL_ALIGNED(32, float, src0, [LEN]);
    LOCAL_ALIGNED(32, float, src1, [LEN]);
    LOCAL_ALIGNED(32, float, src2, [LEN]);
    LOCAL_ALIGNED(32, double, dbl_src0, [LEN]);
    LOCAL_ALIGNED(32, double, dbl_src1, [LEN]);
    for (;;) {
        int arg = getopt(argc, argv, "s:c:");
        if (arg == -1)
            break;
        switch (arg) {
        case 's':
            seed = strtoul(optarg, NULL, 10);
            seeded = 1;
            break;
        case 'c':
        {
            int cpuflags = av_get_cpu_flags();
            if (av_parse_cpu_caps(&cpuflags, optarg) < 0)
                return 1;
            av_force_cpu_flags(cpuflags);
            break;
        }
        }
    }
    if (!seeded)
        seed = av_get_random_seed();
    av_log(NULL, AV_LOG_INFO, "float_dsp-test: %s %u\n", seeded ? "seed" : "random seed", seed);
    av_lfg_init(&lfg, seed);
    fill_float_array(&lfg, src0, LEN);
    fill_float_array(&lfg, src1, LEN);
    fill_float_array(&lfg, src2, LEN);
    fill_double_array(&lfg, dbl_src0, LEN);
    fill_double_array(&lfg, dbl_src1, LEN);
    avpriv_float_dsp_init(&fdsp, 1);
    av_force_cpu_flags(0);
    avpriv_float_dsp_init(&cdsp, 1);
    if (test_vector_fmul(&fdsp, &cdsp, src0, src1))
        ret -= 1 << 0;
    if (test_vector_fmac_scalar(&fdsp, &cdsp, src2, src0, src1[0]))
        ret -= 1 << 1;
    if (test_vector_fmul_scalar(&fdsp, &cdsp, src0, src1[0]))
        ret -= 1 << 2;
    if (test_vector_fmul_window(&fdsp, &cdsp, src0, src1, src2))
        ret -= 1 << 3;
    if (test_vector_fmul_add(&fdsp, &cdsp, src0, src1, src2))
        ret -= 1 << 4;
    if (test_vector_fmul_reverse(&fdsp, &cdsp, src0, src1))
        ret -= 1 << 5;
    if (test_butterflies_float(&fdsp, &cdsp, src0, src1))
        ret -= 1 << 6;
    if (test_scalarproduct_float(&fdsp, &cdsp, src0, src1))
        ret -= 1 << 7;
    if (test_vector_dmul_scalar(&fdsp, &cdsp, dbl_src0, dbl_src1[0]))
        ret -= 1 << 8;
    return ret;
}
