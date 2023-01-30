av_cold void AAC_RENAME(ff_ps_init)(void) {
    
    static const struct {
        const void *ps_codes, *ps_bits;
        const unsigned int table_size, elem_size;
    } ps_tmp[] = {
        PS_VLC_ROW(huff_iid_df1),
        PS_VLC_ROW(huff_iid_dt1),
        PS_VLC_ROW(huff_iid_df0),
        PS_VLC_ROW(huff_iid_dt0),
        PS_VLC_ROW(huff_icc_df),
        PS_VLC_ROW(huff_icc_dt),
        PS_VLC_ROW(huff_ipd_df),
        PS_VLC_ROW(huff_ipd_dt),
        PS_VLC_ROW(huff_opd_df),
        PS_VLC_ROW(huff_opd_dt),
    };
    PS_INIT_VLC_STATIC(0, 1544);
    PS_INIT_VLC_STATIC(1,  832);
    PS_INIT_VLC_STATIC(2, 1024);
    PS_INIT_VLC_STATIC(3, 1036);
    PS_INIT_VLC_STATIC(4,  544);
    PS_INIT_VLC_STATIC(5,  544);
    PS_INIT_VLC_STATIC(6,  512);
    PS_INIT_VLC_STATIC(7,  512);
    PS_INIT_VLC_STATIC(8,  512);
    PS_INIT_VLC_STATIC(9,  512);
    ps_tableinit();
}
