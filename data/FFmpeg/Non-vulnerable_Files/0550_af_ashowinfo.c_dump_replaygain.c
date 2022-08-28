static void dump_replaygain(AVFilterContext *ctx, AVFrameSideData *sd)
{
    AVReplayGain *rg;
    av_log(ctx, AV_LOG_INFO, "replaygain: ");
    if (sd->size < sizeof(*rg)) {
        av_log(ctx, AV_LOG_INFO, "invalid data");
        return;
    }
    rg = (AVReplayGain*)sd->data;
    print_gain(ctx, "track gain", rg->track_gain);
    print_peak(ctx, "track peak", rg->track_peak);
    print_gain(ctx, "album gain", rg->album_gain);
    print_peak(ctx, "album peak", rg->album_peak);
}
