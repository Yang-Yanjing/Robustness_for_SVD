static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    VolumeContext *vol   = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    vol->sample_fmt = inlink->format;
    vol->channels   = inlink->channels;
    vol->planes     = av_sample_fmt_is_planar(inlink->format) ? vol->channels : 1;
    vol->var_values[VAR_N] =
    vol->var_values[VAR_NB_CONSUMED_SAMPLES] =
    vol->var_values[VAR_NB_SAMPLES] =
    vol->var_values[VAR_POS] =
    vol->var_values[VAR_PTS] =
    vol->var_values[VAR_STARTPTS] =
    vol->var_values[VAR_STARTT] =
    vol->var_values[VAR_T] =
    vol->var_values[VAR_VOLUME] = NAN;
    vol->var_values[VAR_NB_CHANNELS] = inlink->channels;
    vol->var_values[VAR_TB]          = av_q2d(inlink->time_base);
    vol->var_values[VAR_SAMPLE_RATE] = inlink->sample_rate;
    av_log(inlink->src, AV_LOG_VERBOSE, "tb:%f sample_rate:%f nb_channels:%f\n",
           vol->var_values[VAR_TB],
           vol->var_values[VAR_SAMPLE_RATE],
           vol->var_values[VAR_NB_CHANNELS]);
    return set_volume(ctx);
}
