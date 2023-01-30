static void sbr_qmf_synthesis(FFTContext *mdct,
                              SBRDSPContext *sbrdsp, AVFixedDSPContext *dsp,
                              INTFLOAT *out, INTFLOAT X,
                              INTFLOAT mdct_buf,
                              INTFLOAT *v0, int *v_off, const unsigned int div)
{
    int i, n;
    const INTFLOAT *sbr_qmf_window = div ? sbr_qmf_window_ds : sbr_qmf_window_us;
    const int step = 128 >> div;
    INTFLOAT *v;
    for (i = 0; i < 32; i++) {
        if (*v_off < step) {
            int saved_samples = (1280 - 128) >> div;
            memcpy(&v0[SBR_SYNTHESIS_BUF_SIZE - saved_samples], v0, saved_samples * sizeof(INTFLOAT));
            *v_off = SBR_SYNTHESIS_BUF_SIZE - saved_samples - step;
        } else {
            *v_off -= step;
        }
        v = v0 + *v_off;
        if (div) {
            for (n = 0; n < 32; n++) {
                X[0][i][   n] = -X[0][i][n];
                X[0][i][32+n] =  X[1][i][31-n];
            }
            mdct->imdct_half(mdct, mdct_buf[0], X[0][i]);
            sbrdsp->qmf_deint_neg(v, mdct_buf[0]);
        } else {
            sbrdsp->neg_odd_64(X[1][i]);
            mdct->imdct_half(mdct, mdct_buf[0], X[0][i]);
            mdct->imdct_half(mdct, mdct_buf[1], X[1][i]);
            sbrdsp->qmf_deint_bfly(v, mdct_buf[1], mdct_buf[0]);
        }
        dsp->vector_fmul    (out, v                , sbr_qmf_window                       , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 192 >> div), sbr_qmf_window + ( 64 >> div), out   , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 256 >> div), sbr_qmf_window + (128 >> div), out   , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 448 >> div), sbr_qmf_window + (192 >> div), out   , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 512 >> div), sbr_qmf_window + (256 >> div), out   , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 704 >> div), sbr_qmf_window + (320 >> div), out   , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 768 >> div), sbr_qmf_window + (384 >> div), out   , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 960 >> div), sbr_qmf_window + (448 >> div), out   , 64 >> div);
        dsp->vector_fmul_add(out, v + (1024 >> div), sbr_qmf_window + (512 >> div), out   , 64 >> div);
        dsp->vector_fmul_add(out, v + (1216 >> div), sbr_qmf_window + (576 >> div), out   , 64 >> div);
        out += 64 >> div;
    }
}