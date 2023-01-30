void ff_aac_encode_main_pred(AACEncContext *s, SingleChannelElement *sce)
{
    int sfb;
    IndividualChannelStream *ics = &sce->ics;
    const int pmax = FFMIN(ics->max_sfb, ff_aac_pred_sfb_max[s->samplerate_index]);
    if (!ics->predictor_present)
        return;
    put_bits(&s->pb, 1, !!ics->predictor_reset_group);
    if (ics->predictor_reset_group)
        put_bits(&s->pb, 5, ics->predictor_reset_group);
    for (sfb = 0; sfb < pmax; sfb++)
        put_bits(&s->pb, 1, ics->prediction_used[sfb]);
}
