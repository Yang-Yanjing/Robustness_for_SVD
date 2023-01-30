static void calc_predictor_params(AlacEncodeContext *s, int ch)
{
    int32_t coefs[MAX_LPC_ORDER][MAX_LPC_ORDER];
    int shift[MAX_LPC_ORDER];
    int opt_order;
    if (s->compression_level == 1) {
        s->lpc[ch].lpc_order = 6;
        s->lpc[ch].lpc_quant = 6;
        s->lpc[ch].lpc_coeff[0] =  160;
        s->lpc[ch].lpc_coeff[1] = -190;
        s->lpc[ch].lpc_coeff[2] =  170;
        s->lpc[ch].lpc_coeff[3] = -130;
        s->lpc[ch].lpc_coeff[4] =   80;
        s->lpc[ch].lpc_coeff[5] =  -25;
    } else {
        opt_order = ff_lpc_calc_coefs(&s->lpc_ctx, s->sample_buf[ch],
                                      s->frame_size,
                                      s->min_prediction_order,
                                      s->max_prediction_order,
                                      ALAC_MAX_LPC_PRECISION, coefs, shift,
                                      FF_LPC_TYPE_LEVINSON, 0,
                                      ORDER_METHOD_EST, ALAC_MAX_LPC_SHIFT, 1);
        s->lpc[ch].lpc_order = opt_order;
        s->lpc[ch].lpc_quant = shift[opt_order-1];
        memcpy(s->lpc[ch].lpc_coeff, coefs[opt_order-1], opt_order*sizeof(int));
    }
}
