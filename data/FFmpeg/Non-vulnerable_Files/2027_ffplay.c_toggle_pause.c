static void toggle_pause(VideoState *is)
{
    stream_toggle_pause(is);
    is->step = 0;
}
