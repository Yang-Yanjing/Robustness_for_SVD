static void put_ics_info(AACEncContext *s, IndividualChannelStream *info)
{
    int w;
    put_bits(&s->pb, 1, 0);                
    put_bits(&s->pb, 2, info->window_sequence[0]);
    put_bits(&s->pb, 1, info->use_kb_window[0]);
    if (info->window_sequence[0] != EIGHT_SHORT_SEQUENCE) {
        put_bits(&s->pb, 6, info->max_sfb);
        put_bits(&s->pb, 1, !!info->predictor_present);
    } else {
        put_bits(&s->pb, 4, info->max_sfb);
        for (w = 1; w < 8; w++)
            put_bits(&s->pb, 1, !info->group_len[w]);
    }
}
