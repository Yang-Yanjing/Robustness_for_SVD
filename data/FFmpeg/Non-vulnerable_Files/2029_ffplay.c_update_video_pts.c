static void update_video_pts(VideoState *is, double pts, int64_t pos, int serial) {
    
    set_clock(&is->vidclk, pts, serial);
    sync_clock_to_slave(&is->extclk, &is->vidclk);
}
