static CueDesc get_cue_desc(AVFormatContext *s, int64_t ts, int64_t cues_start) {
    MatroskaDemuxContext *matroska = s->priv_data;
    CueDesc cue_desc;
    int i;
    int nb_index_entries = s->streams[0]->nb_index_entries;
    AVIndexEntry *index_entries = s->streams[0]->index_entries;
    if (ts >= matroska->duration * matroska->time_scale) return (CueDesc) {-1, -1, -1, -1};
    for (i = 1; i < nb_index_entries; i++) {
        if (index_entries[i - 1].timestamp * matroska->time_scale <= ts &&
            index_entries[i].timestamp * matroska->time_scale > ts) {
            break;
        }
    }
    --i;
    cue_desc.start_time_ns = index_entries[i].timestamp * matroska->time_scale;
    cue_desc.start_offset = index_entries[i].pos - matroska->segment_start;
    if (i != nb_index_entries - 1) {
        cue_desc.end_time_ns = index_entries[i + 1].timestamp * matroska->time_scale;
        cue_desc.end_offset = index_entries[i + 1].pos - matroska->segment_start;
    } else {
        cue_desc.end_time_ns = matroska->duration * matroska->time_scale;
        
        
        
        cue_desc.end_offset = cues_start - matroska->segment_start;
    }
    return cue_desc;
}
