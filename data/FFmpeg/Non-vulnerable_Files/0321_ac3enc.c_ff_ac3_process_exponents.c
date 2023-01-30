void ff_ac3_process_exponents(AC3EncodeContext *s)
{
    extract_exponents(s);
    compute_exp_strategy(s);
    encode_exponents(s);
    emms_c();
}
