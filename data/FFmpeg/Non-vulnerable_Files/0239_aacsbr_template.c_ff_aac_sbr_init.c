av_cold void AAC_RENAME(ff_aac_sbr_init)(void)
{
    static const struct {
        const void *sbr_codes, *sbr_bits;
        const unsigned int table_size, elem_size;
    } sbr_tmp[] = {
        SBR_VLC_ROW(t_huffman_env_1_5dB),
        SBR_VLC_ROW(f_huffman_env_1_5dB),
        SBR_VLC_ROW(t_huffman_env_bal_1_5dB),
        SBR_VLC_ROW(f_huffman_env_bal_1_5dB),
        SBR_VLC_ROW(t_huffman_env_3_0dB),
        SBR_VLC_ROW(f_huffman_env_3_0dB),
        SBR_VLC_ROW(t_huffman_env_bal_3_0dB),
        SBR_VLC_ROW(f_huffman_env_bal_3_0dB),
        SBR_VLC_ROW(t_huffman_noise_3_0dB),
        SBR_VLC_ROW(t_huffman_noise_bal_3_0dB),
    };
    
    SBR_INIT_VLC_STATIC(0, 1098);
    SBR_INIT_VLC_STATIC(1, 1092);
    SBR_INIT_VLC_STATIC(2, 768);
    SBR_INIT_VLC_STATIC(3, 1026);
    SBR_INIT_VLC_STATIC(4, 1058);
    SBR_INIT_VLC_STATIC(5, 1052);
    SBR_INIT_VLC_STATIC(6, 544);
    SBR_INIT_VLC_STATIC(7, 544);
    SBR_INIT_VLC_STATIC(8, 592);
    SBR_INIT_VLC_STATIC(9, 512);
    aacsbr_tableinit();
    AAC_RENAME(ff_ps_init)();
}
