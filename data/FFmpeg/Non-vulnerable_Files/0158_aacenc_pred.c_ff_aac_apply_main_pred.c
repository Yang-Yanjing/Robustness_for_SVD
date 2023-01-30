void ff_aac_apply_main_pred(AACEncContext *s, SingleChannelElement *sce)
{
    int sfb, k;
    const int pmax = FFMIN(sce->ics.max_sfb, ff_aac_pred_sfb_max[s->samplerate_index]);
    if (sce->ics.window_sequence[0] != EIGHT_SHORT_SEQUENCE) {
        for (sfb = 0; sfb < pmax; sfb++) {
            for (k = sce->ics.swb_offset[sfb]; k < sce->ics.swb_offset[sfb + 1]; k++) {
                predict(&sce->predictor_state[k], &sce->coeffs[k], &sce->prcoeffs[k],
                        sce->ics.predictor_present && sce->ics.prediction_used[sfb]);
            }
        }
        if (sce->ics.predictor_reset_group) {
            reset_predictor_group(sce, sce->ics.predictor_reset_group);
        }
    } else {
        reset_all_predictors(sce->predictor_state);
    }
}
