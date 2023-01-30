static int mxf_compute_ptses_fake_index(MXFContext *mxf, MXFIndexTable *index_table)
{
    int i, j, x;
    int8_t max_temporal_offset = -128;
    uint8_t *flags;
    
    for (i = 0; i < index_table->nb_segments; i++) {
        MXFIndexTableSegment *s = index_table->segments[i];
        if (!s->nb_index_entries) {
            index_table->nb_ptses = 0;
            return 0;                               
        }
        index_table->nb_ptses += s->index_duration;
    }
    
    if (index_table->nb_ptses <= 0)
        return 0;
    if (!(index_table->ptses      = av_calloc(index_table->nb_ptses, sizeof(int64_t))) ||
        !(index_table->fake_index = av_calloc(index_table->nb_ptses, sizeof(AVIndexEntry))) ||
        !(index_table->offsets    = av_calloc(index_table->nb_ptses, sizeof(int8_t))) ||
        !(flags                   = av_calloc(index_table->nb_ptses, sizeof(uint8_t)))) {
        av_freep(&index_table->ptses);
        av_freep(&index_table->fake_index);
        av_freep(&index_table->offsets);
        return AVERROR(ENOMEM);
    }
    

    for (x = 0; x < index_table->nb_ptses; x++)
        index_table->ptses[x] = AV_NOPTS_VALUE;
    


























    for (i = x = 0; i < index_table->nb_segments; i++) {
        MXFIndexTableSegment *s = index_table->segments[i];
        int index_delta = 1;
        int n = s->nb_index_entries;
        if (s->nb_index_entries == 2 * s->index_duration + 1) {
            index_delta = 2;    
            
            n--;
        }
        for (j = 0; j < n; j += index_delta, x++) {
            int offset = s->temporal_offset_entries[j] / index_delta;
            int index  = x + offset;
            if (x >= index_table->nb_ptses) {
                av_log(mxf->fc, AV_LOG_ERROR,
                       "x >= nb_ptses - IndexEntryCount %i < IndexDuration %"PRId64"?\n",
                       s->nb_index_entries, s->index_duration);
                break;
            }
            flags[x] = !(s->flag_entries[j] & 0x30) ? AVINDEX_KEYFRAME : 0;
            if (index < 0 || index >= index_table->nb_ptses) {
                av_log(mxf->fc, AV_LOG_ERROR,
                       "index entry %i + TemporalOffset %i = %i, which is out of bounds\n",
                       x, offset, index);
                continue;
            }
            index_table->offsets[x] = offset;
            index_table->ptses[index] = x;
            max_temporal_offset = FFMAX(max_temporal_offset, offset);
        }
    }
    
    for (x = 0; x < index_table->nb_ptses; x++) {
        index_table->fake_index[x].timestamp = x;
        if (index_table->ptses[x] != AV_NOPTS_VALUE)
            index_table->fake_index[index_table->ptses[x]].flags = flags[x];
    }
    av_freep(&flags);
    index_table->first_dts = -max_temporal_offset;
    return 0;
}
