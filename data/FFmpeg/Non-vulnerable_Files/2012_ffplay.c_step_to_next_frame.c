static void step_to_next_frame(VideoState *is)
{
    
    if (is->paused)
        stream_toggle_pause(is);
    is->step = 1;
}
