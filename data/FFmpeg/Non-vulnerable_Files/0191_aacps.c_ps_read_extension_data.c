READ_PAR_DATA(ipdopd,                      0, 0x07, 0)
static int ps_read_extension_data(GetBitContext *gb, PSContext *ps, int ps_extension_id)
{
    int e;
    int count = get_bits_count(gb);
    if (ps_extension_id)
        return 0;
    ps->enable_ipdopd = get_bits1(gb);
    if (ps->enable_ipdopd) {
        for (e = 0; e < ps->num_env; e++) {
            int dt = get_bits1(gb);
            read_ipdopd_data(NULL, gb, ps, ps->ipd_par, dt ? huff_ipd_dt : huff_ipd_df, e, dt);
            dt = get_bits1(gb);
            read_ipdopd_data(NULL, gb, ps, ps->opd_par, dt ? huff_opd_dt : huff_opd_df, e, dt);
        }
    }
    skip_bits1(gb);      
    return get_bits_count(gb) - count;
}
