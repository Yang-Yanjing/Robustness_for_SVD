int ff_framesync_configure(FFFrameSync *fs)
{
    unsigned i;
    int64_t gcd, lcm;
    if (!fs->time_base.num) {
        for (i = 0; i < fs->nb_in; i++) {
            if (fs->in[i].sync) {
                if (fs->time_base.num) {
                    gcd = av_gcd(fs->time_base.den, fs->in[i].time_base.den);
                    lcm = (fs->time_base.den / gcd) * fs->in[i].time_base.den;
                    if (lcm < AV_TIME_BASE / 2) {
                        fs->time_base.den = lcm;
                        fs->time_base.num = av_gcd(fs->time_base.num,
                                                   fs->in[i].time_base.num);
                    } else {
                        fs->time_base.num = 1;
                        fs->time_base.den = AV_TIME_BASE;
                        break;
                    }
                } else {
                    fs->time_base = fs->in[i].time_base;
                }
            }
        }
        if (!fs->time_base.num) {
            av_log(fs, AV_LOG_ERROR, "Impossible to set time base\n");
            return AVERROR(EINVAL);
        }
        av_log(fs, AV_LOG_VERBOSE, "Selected %d/%d time base\n",
               fs->time_base.num, fs->time_base.den);
    }
    for (i = 0; i < fs->nb_in; i++)
        fs->in[i].pts = fs->in[i].pts_next = AV_NOPTS_VALUE;
    fs->sync_level = UINT_MAX;
    framesync_sync_level_update(fs);
    return 0;
}
