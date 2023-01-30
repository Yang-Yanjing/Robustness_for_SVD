static void dump_downmix(AVFilterContext *ctx, AVFrameSideData *sd)
{
    AVDownmixInfo *di;
    av_log(ctx, AV_LOG_INFO, "downmix: ");
    if (sd->size < sizeof(*di)) {
        av_log(ctx, AV_LOG_INFO, "invalid data");
        return;
    }
    di = (AVDownmixInfo *)sd->data;
    av_log(ctx, AV_LOG_INFO, "preferred downmix type - ");
    switch (di->preferred_downmix_type) {
    case AV_DOWNMIX_TYPE_LORO:    av_log(ctx, AV_LOG_INFO, "Lo/Ro");              break;
    case AV_DOWNMIX_TYPE_LTRT:    av_log(ctx, AV_LOG_INFO, "Lt/Rt");              break;
    case AV_DOWNMIX_TYPE_DPLII:   av_log(ctx, AV_LOG_INFO, "Dolby Pro Logic II"); break;
    default:                      av_log(ctx, AV_LOG_WARNING, "unknown");         break;
    }
    av_log(ctx, AV_LOG_INFO, " Mix levels: center %f (%f ltrt) - "
           "surround %f (%f ltrt) - lfe %f",
           di->center_mix_level, di->center_mix_level_ltrt,
           di->surround_mix_level, di->surround_mix_level_ltrt,
           di->lfe_mix_level);
}
