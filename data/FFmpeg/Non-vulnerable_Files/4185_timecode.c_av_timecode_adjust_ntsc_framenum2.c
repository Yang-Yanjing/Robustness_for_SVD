int av_timecode_adjust_ntsc_framenum2(int framenum, int fps)
{
    
    int drop_frames = 0;
    int d, m, frames_per_10mins;
    if (fps == 30) {
        drop_frames = 2;
        frames_per_10mins = 17982;
    } else if (fps == 60) {
        drop_frames = 4;
        frames_per_10mins = 35964;
    } else
        return framenum;
    d = framenum / frames_per_10mins;
    m = framenum % frames_per_10mins;
    return framenum + 9 * drop_frames * d + drop_frames * ((m - drop_frames) / (frames_per_10mins / 10));
}
