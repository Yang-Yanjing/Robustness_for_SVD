
static void sbr_make_f_tablelim(SpectralBandReplication *sbr)
{
    int k;
    if (sbr->bs_limiter_bands > 0) {
        static const INTFLOAT bands_warped[3] = { Q23(1.32715174233856803909f),   
                                               Q23(1.18509277094158210129f),   
                                               Q23(1.11987160404675912501f) }; 
        const INTFLOAT lim_bands_per_octave_warped = bands_warped[sbr->bs_limiter_bands - 1];
        int16_t patch_borders[7];
        uint16_t *in = sbr->f_tablelim + 1, *out = sbr->f_tablelim;
        patch_borders[0] = sbr->kx[1];
        for (k = 1; k <= sbr->num_patches; k++)
            patch_borders[k] = patch_borders[k-1] + sbr->patch_num_subbands[k-1];
        memcpy(sbr->f_tablelim, sbr->f_tablelow,
               (sbr->n[0] + 1) * sizeof(sbr->f_tablelow[0]));
        if (sbr->num_patches > 1)
            memcpy(sbr->f_tablelim + sbr->n[0] + 1, patch_borders + 1,
                   (sbr->num_patches - 1) * sizeof(patch_borders[0]));
        qsort(sbr->f_tablelim, sbr->num_patches + sbr->n[0],
              sizeof(sbr->f_tablelim[0]),
              qsort_comparison_function_int16);
        sbr->n_lim = sbr->n[0] + sbr->num_patches - 1;
        while (out < sbr->f_tablelim + sbr->n_lim) {
#if USE_FIXED
            if ((*in << 23) >= *out * lim_bands_per_octave_warped) {
#else
            if (*in >= *out * lim_bands_per_octave_warped) {
#endif 
                *++out = *in++;
            } else if (*in == *out ||
                !in_table_int16(patch_borders, sbr->num_patches, *in)) {
                in++;
                sbr->n_lim--;
            } else if (!in_table_int16(patch_borders, sbr->num_patches, *out)) {
                *out = *in++;
                sbr->n_lim--;
            } else {
                *++out = *in++;
            }
        }
    } else {
        sbr->f_tablelim[0] = sbr->f_tablelow[0];
        sbr->f_tablelim[1] = sbr->f_tablelow[sbr->n[0]];
        sbr->n_lim = 1;
    }
}
