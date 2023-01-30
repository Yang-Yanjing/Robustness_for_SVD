char *av_timecode_make_string(const AVTimecode *tc, char *buf, int framenum)
{
    int fps = tc->fps;
    int drop = tc->flags & AV_TIMECODE_FLAG_DROPFRAME;
    int hh, mm, ss, ff, neg = 0;
    framenum += tc->start;
    if (drop)
        framenum = av_timecode_adjust_ntsc_framenum2(framenum, fps);
    if (framenum < 0) {
        framenum = -framenum;
        neg = tc->flags & AV_TIMECODE_FLAG_ALLOWNEGATIVE;
    }
    ff = framenum % fps;
    ss = framenum / fps        % 60;
    mm = framenum / (fps*60)   % 60;
    hh = framenum / (fps*3600);
    if (tc->flags & AV_TIMECODE_FLAG_24HOURSMAX)
        hh = hh % 24;
    snprintf(buf, AV_TIMECODE_STR_SIZE, "%s%02d:%02d:%02d%c%02d",
             neg ? "-" : "",
             hh, mm, ss, drop ? ';' : ':', ff);
    return buf;
}
