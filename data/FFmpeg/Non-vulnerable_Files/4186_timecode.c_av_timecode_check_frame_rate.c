int av_timecode_check_frame_rate(AVRational rate)
{
    return check_fps(fps_from_frame_rate(rate));
}
