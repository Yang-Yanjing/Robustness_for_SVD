static int mkv_add_cuepoint(mkv_cues *cues, int stream, int tracknum, int64_t ts,
                            int64_t cluster_pos, int64_t relative_pos, int64_t duration)
{
    mkv_cuepoint *entries = cues->entries;
    if (ts < 0)
        return 0;
    entries = av_realloc_array(entries, cues->num_entries + 1, sizeof(mkv_cuepoint));
    if (!entries)
        return AVERROR(ENOMEM);
    cues->entries = entries;
    cues->entries[cues->num_entries].pts           = ts;
    cues->entries[cues->num_entries].stream_idx    = stream;
    cues->entries[cues->num_entries].tracknum      = tracknum;
    cues->entries[cues->num_entries].cluster_pos   = cluster_pos - cues->segment_offset;
    cues->entries[cues->num_entries].relative_pos  = relative_pos;
    cues->entries[cues->num_entries++].duration    = duration;
    return 0;
}
