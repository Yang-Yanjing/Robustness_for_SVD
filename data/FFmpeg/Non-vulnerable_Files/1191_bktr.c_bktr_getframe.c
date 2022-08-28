static void bktr_getframe(uint64_t per_frame)
{
    uint64_t curtime;
    curtime = av_gettime();
    if (!last_frame_time
        || ((last_frame_time + per_frame) > curtime)) {
        if (!usleep(last_frame_time + per_frame + per_frame / 8 - curtime)) {
            if (!nsignals)
                av_log(NULL, AV_LOG_INFO,
                       "SLEPT NO signals - %d microseconds late\n",
                       (int)(av_gettime() - last_frame_time - per_frame));
        }
    }
    nsignals = 0;
    last_frame_time = curtime;
}
