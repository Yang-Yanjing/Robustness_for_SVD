void ff_framesync_drop(FFFrameSync *fs)
{
    fs->frame_ready = 0;
}
