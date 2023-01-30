static void ac3_exponent_min_c(uint8_t *exp, int num_reuse_blocks, int nb_coefs)
{
    int blk, i;
    if (!num_reuse_blocks)
        return;
    for (i = 0; i < nb_coefs; i++) {
        uint8_t min_exp = *exp;
        uint8_t *exp1 = exp + 256;
        for (blk = 0; blk < num_reuse_blocks; blk++) {
            uint8_t next_exp = *exp1;
            if (next_exp < min_exp)
                min_exp = next_exp;
            exp1 += 256;
        }
        *exp++ = min_exp;
    }
}
