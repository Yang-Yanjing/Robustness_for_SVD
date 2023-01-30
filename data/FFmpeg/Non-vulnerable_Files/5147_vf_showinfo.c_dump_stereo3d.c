static void dump_stereo3d(AVFilterContext *ctx, AVFrameSideData *sd)
{
    AVStereo3D *stereo;
    av_log(ctx, AV_LOG_INFO, "stereoscopic information: ");
    if (sd->size < sizeof(*stereo)) {
        av_log(ctx, AV_LOG_INFO, "invalid data");
        return;
    }
    stereo = (AVStereo3D *)sd->data;
    av_log(ctx, AV_LOG_INFO, "type - ");
    switch (stereo->type) {
    case AV_STEREO3D_2D:                  av_log(ctx, AV_LOG_INFO, "2D");                     break;
    case AV_STEREO3D_SIDEBYSIDE:          av_log(ctx, AV_LOG_INFO, "side by side");           break;
    case AV_STEREO3D_TOPBOTTOM:           av_log(ctx, AV_LOG_INFO, "top and bottom");         break;
    case AV_STEREO3D_FRAMESEQUENCE:       av_log(ctx, AV_LOG_INFO, "frame alternate");        break;
    case AV_STEREO3D_CHECKERBOARD:        av_log(ctx, AV_LOG_INFO, "checkerboard");           break;
    case AV_STEREO3D_LINES:               av_log(ctx, AV_LOG_INFO, "interleaved lines");      break;
    case AV_STEREO3D_COLUMNS:             av_log(ctx, AV_LOG_INFO, "interleaved columns");    break;
    case AV_STEREO3D_SIDEBYSIDE_QUINCUNX: av_log(ctx, AV_LOG_INFO, "side by side "
                                                                   "(quincunx subsampling)"); break;
    default:                              av_log(ctx, AV_LOG_WARNING, "unknown");             break;
    }
    if (stereo->flags & AV_STEREO3D_FLAG_INVERT)
        av_log(ctx, AV_LOG_INFO, " (inverted)");
}
