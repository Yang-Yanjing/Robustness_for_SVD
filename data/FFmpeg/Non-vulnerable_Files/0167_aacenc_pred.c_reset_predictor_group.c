static inline void reset_predictor_group(SingleChannelElement *sce, int group_num)
{
    int i;
    PredictorState *ps = sce->predictor_state;
    for (i = group_num - 1; i < MAX_PREDICTORS; i += 30)
        reset_predict_state(&ps[i]);
}
