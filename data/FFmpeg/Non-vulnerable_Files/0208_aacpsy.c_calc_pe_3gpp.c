static float calc_pe_3gpp(AacPsyBand *band)
{
    float pe, a;
    band->pe           = 0.0f;
    band->pe_const     = 0.0f;
    band->active_lines = 0.0f;
    if (band->energy > band->thr) {
        a  = log2f(band->energy);
        pe = a - log2f(band->thr);
        band->active_lines = band->nz_lines;
        if (pe < PSY_3GPP_C1) {
            pe = pe * PSY_3GPP_C3 + PSY_3GPP_C2;
            a  = a  * PSY_3GPP_C3 + PSY_3GPP_C2;
            band->active_lines *= PSY_3GPP_C3;
        }
        band->pe       = pe * band->nz_lines;
        band->pe_const = a  * band->nz_lines;
    }
    return band->pe;
}
