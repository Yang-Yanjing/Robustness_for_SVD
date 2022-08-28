static int64_t read_trun_duration(AVIOContext *in, int default_duration,
                                  int64_t end)
{
    int64_t dts = 0;
    int64_t pos;
    int flags, i;
    int entries;
    int64_t first_pts = 0;
    int64_t max_pts = 0;
    avio_r8(in); 
    flags = avio_rb24(in);
    if (default_duration <= 0 && !(flags & MOV_TRUN_SAMPLE_DURATION)) {
        fprintf(stderr, "No sample duration in trun flags\n");
        return -1;
    }
    entries = avio_rb32(in);
    if (flags & MOV_TRUN_DATA_OFFSET)        avio_rb32(in);
    if (flags & MOV_TRUN_FIRST_SAMPLE_FLAGS) avio_rb32(in);
    pos = avio_tell(in);
    for (i = 0; i < entries && pos < end; i++) {
        int sample_duration = default_duration;
        int64_t pts = dts;
        if (flags & MOV_TRUN_SAMPLE_DURATION) sample_duration = avio_rb32(in);
        if (flags & MOV_TRUN_SAMPLE_SIZE)     avio_rb32(in);
        if (flags & MOV_TRUN_SAMPLE_FLAGS)    avio_rb32(in);
        if (flags & MOV_TRUN_SAMPLE_CTS)      pts += avio_rb32(in);
        if (sample_duration < 0) {
            fprintf(stderr, "Negative sample duration %d\n", sample_duration);
            return -1;
        }
        if (i == 0)
            first_pts = pts;
        max_pts = FFMAX(max_pts, pts + sample_duration);
        dts += sample_duration;
        pos = avio_tell(in);
    }
    return max_pts - first_pts;
}
