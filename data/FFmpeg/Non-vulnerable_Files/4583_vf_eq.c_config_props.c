static int config_props(AVFilterLink *inlink)
{
    EQContext *eq = inlink->dst->priv;
    eq->var_values[VAR_N] = 0;
    eq->var_values[VAR_R] = inlink->frame_rate.num == 0 || inlink->frame_rate.den == 0 ?
        NAN : av_q2d(inlink->frame_rate);
    return 0;
}
