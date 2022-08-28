static inline int update_counters(IndividualChannelStream *ics, int inc)
{
    int i;
    for (i = 1; i < 31; i++) {
        ics->predictor_reset_count[i] += inc;
        if (ics->predictor_reset_count[i] > PRED_RESET_FRAME_MIN)
            return i; 
    }
    return 0;
}
