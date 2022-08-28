static void sbr_turnoff(SpectralBandReplication *sbr) {
    sbr->start = 0;
    
    sbr->kx[1] = 32; 
    sbr->m[1] = 0;
    
    sbr->data[0].e_a[1] = sbr->data[1].e_a[1] = -1;
    memset(&sbr->spectrum_params, -1, sizeof(SpectrumParameters));
}
