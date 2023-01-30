static int config_input(AVFilterLink *inlink)
{
    SelectContext *select = inlink->dst->priv;
    select->var_values[VAR_N]          = 0.0;
    select->var_values[VAR_SELECTED_N] = 0.0;
    select->var_values[VAR_TB] = av_q2d(inlink->time_base);
    select->var_values[VAR_PREV_PTS]          = NAN;
    select->var_values[VAR_PREV_SELECTED_PTS] = NAN;
    select->var_values[VAR_PREV_SELECTED_T]   = NAN;
    select->var_values[VAR_PREV_T]            = NAN;
    select->var_values[VAR_START_PTS]         = NAN;
    select->var_values[VAR_START_T]           = NAN;
    select->var_values[VAR_I]  = AV_PICTURE_TYPE_I;
    select->var_values[VAR_P]  = AV_PICTURE_TYPE_P;
    select->var_values[VAR_B]  = AV_PICTURE_TYPE_B;
    select->var_values[VAR_SI] = AV_PICTURE_TYPE_SI;
    select->var_values[VAR_SP] = AV_PICTURE_TYPE_SP;
    select->var_values[VAR_BI] = AV_PICTURE_TYPE_BI;
    select->var_values[VAR_PICT_TYPE_I]  = AV_PICTURE_TYPE_I;
    select->var_values[VAR_PICT_TYPE_P]  = AV_PICTURE_TYPE_P;
    select->var_values[VAR_PICT_TYPE_B]  = AV_PICTURE_TYPE_B;
    select->var_values[VAR_PICT_TYPE_SI] = AV_PICTURE_TYPE_SI;
    select->var_values[VAR_PICT_TYPE_SP] = AV_PICTURE_TYPE_SP;
    select->var_values[VAR_PICT_TYPE_BI] = AV_PICTURE_TYPE_BI;
    select->var_values[VAR_INTERLACE_TYPE_P] = INTERLACE_TYPE_P;
    select->var_values[VAR_INTERLACE_TYPE_T] = INTERLACE_TYPE_T;
    select->var_values[VAR_INTERLACE_TYPE_B] = INTERLACE_TYPE_B;
    select->var_values[VAR_PICT_TYPE]         = NAN;
    select->var_values[VAR_INTERLACE_TYPE]    = NAN;
    select->var_values[VAR_SCENE]             = NAN;
    select->var_values[VAR_CONSUMED_SAMPLES_N] = NAN;
    select->var_values[VAR_SAMPLES_N]          = NAN;
    select->var_values[VAR_SAMPLE_RATE] =
        inlink->type == AVMEDIA_TYPE_AUDIO ? inlink->sample_rate : NAN;
    if (select->do_scene_detect) {
        select->sad = av_pixelutils_get_sad_fn(3, 3, 2, select); 
        if (!select->sad)
            return AVERROR(EINVAL);
    }
    return 0;
}
