void ff_aac_encode_tns_info(AACEncContext *s, SingleChannelElement *sce)
{
    uint8_t u_coef;
    const uint8_t coef_res = TNS_Q_BITS == 4;
    int i, w, filt, coef_len, coef_compress = 0;
    const int is8 = sce->ics.window_sequence[0] == EIGHT_SHORT_SEQUENCE;
    TemporalNoiseShaping *tns = &sce->tns;
    if (!sce->tns.present)
        return;
    for (i = 0; i < sce->ics.num_windows; i++) {
        put_bits(&s->pb, 2 - is8, sce->tns.n_filt[i]);
        if (tns->n_filt[i]) {
            put_bits(&s->pb, 1, coef_res);
            for (filt = 0; filt < tns->n_filt[i]; filt++) {
                put_bits(&s->pb, 6 - 2 * is8, tns->length[i][filt]);
                put_bits(&s->pb, 5 - 2 * is8, tns->order[i][filt]);
                if (tns->order[i][filt]) {
                    put_bits(&s->pb, 1, !!tns->direction[i][filt]);
                    put_bits(&s->pb, 1, !!coef_compress);
                    coef_len = coef_res + 3 - coef_compress;
                    for (w = 0; w < tns->order[i][filt]; w++) {
                        u_coef = (tns->coef_idx[i][filt][w])&(~(~0<<coef_len));
                        put_bits(&s->pb, coef_len, u_coef);
                    }
                }
            }
        }
    }
}
