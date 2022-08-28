static av_cold void ac3_tables_init(void)
{
    int i;
    

    for (i = 0; i < 128; i++) {
        ungroup_3_in_7_bits_tab[i][0] =  i / 25;
        ungroup_3_in_7_bits_tab[i][1] = (i % 25) / 5;
        ungroup_3_in_7_bits_tab[i][2] = (i % 25) % 5;
    }
    

    for (i = 0; i < 32; i++) {
        
        b1_mantissas[i][0] = symmetric_dequant(ff_ac3_ungroup_3_in_5_bits_tab[i][0], 3);
        b1_mantissas[i][1] = symmetric_dequant(ff_ac3_ungroup_3_in_5_bits_tab[i][1], 3);
        b1_mantissas[i][2] = symmetric_dequant(ff_ac3_ungroup_3_in_5_bits_tab[i][2], 3);
    }
    for (i = 0; i < 128; i++) {
        
        b2_mantissas[i][0] = symmetric_dequant(ungroup_3_in_7_bits_tab[i][0], 5);
        b2_mantissas[i][1] = symmetric_dequant(ungroup_3_in_7_bits_tab[i][1], 5);
        b2_mantissas[i][2] = symmetric_dequant(ungroup_3_in_7_bits_tab[i][2], 5);
        
        b4_mantissas[i][0] = symmetric_dequant(i / 11, 11);
        b4_mantissas[i][1] = symmetric_dequant(i % 11, 11);
    }
    

    for (i = 0; i < 7; i++) {
        
        b3_mantissas[i] = symmetric_dequant(i, 7);
    }
    for (i = 0; i < 15; i++) {
        
        b5_mantissas[i] = symmetric_dequant(i, 15);
    }
    

    for (i = 0; i < 256; i++) {
        int v = (i >> 5) - ((i >> 7) << 3) - 5;
        dynamic_range_tab[i] = powf(2.0f, v) * ((i & 0x1F) | 0x20);
    }
    

    for (i = 0; i < 256; i++) {
        int v = (i >> 4) - ((i >> 7) << 4) - 4;
        heavy_dynamic_range_tab[i] = powf(2.0f, v) * ((i & 0xF) | 0x10);
    }
}
