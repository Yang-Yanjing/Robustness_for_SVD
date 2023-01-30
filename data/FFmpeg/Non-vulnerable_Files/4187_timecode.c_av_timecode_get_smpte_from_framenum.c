uint32_t av_timecode_get_smpte_from_framenum(const AVTimecode *tc, int framenum)
{
    unsigned fps = tc->fps;
    int drop = !!(tc->flags & AV_TIMECODE_FLAG_DROPFRAME);
    int hh, mm, ss, ff;
    framenum += tc->start;
    if (drop)
        framenum = av_timecode_adjust_ntsc_framenum2(framenum, tc->fps);
    ff = framenum % fps;
    ss = framenum / fps      % 60;
    mm = framenum / (fps*60) % 60;
    hh = framenum / (fps*3600) % 24;
    return 0         << 31 | 
           drop      << 30 | 
           (ff / 10) << 28 | 
           (ff % 10) << 24 | 
           0         << 23 | 
           (ss / 10) << 20 | 
           (ss % 10) << 16 | 
           0         << 15 | 
           (mm / 10) << 12 | 
           (mm % 10) <<  8 | 
           0         <<  7 | 
           0         <<  6 | 
           (hh / 10) <<  4 | 
           (hh % 10);        
}
