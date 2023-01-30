static void skip_scaling_list_data(GetBitContext *gb)
{
    int i, j, k, num_coeffs;
    for (i = 0; i < 4; i++)
        for (j = 0; j < (i == 3 ? 2 : 6); j++)
            if (!get_bits1(gb))         
                get_ue_golomb_long(gb); 
            else {
                num_coeffs = FFMIN(64, 1 << (4 + (i << 1)));
                if (i > 1)
                    get_se_golomb_long(gb); 
                for (k = 0; k < num_coeffs; k++)
                    get_se_golomb_long(gb); 
            }
}
