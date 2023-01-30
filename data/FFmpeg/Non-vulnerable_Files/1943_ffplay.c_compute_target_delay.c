static double compute_target_delay(double delay, VideoState *is)
{
    double sync_threshold, diff = 0;
    
    if (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) {
        

        diff = get_clock(&is->vidclk) - get_master_clock(is);
        


        sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
        if (!isnan(diff) && fabs(diff) < is->max_frame_duration) {
            if (diff <= -sync_threshold)
                delay = FFMAX(0, delay + diff);
            else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
                delay = delay + diff;
            else if (diff >= sync_threshold)
                delay = 2 * delay;
        }
    }
    av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
            delay, -diff);
    return delay;
}
