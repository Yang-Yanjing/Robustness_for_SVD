static void clear_index_entries(AVFormatContext *s, int64_t pos)
{
    int i, j, out;
    av_log(s, AV_LOG_WARNING,
           "Found invalid index entries, clearing the index.\n");
    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        
        out = 0;
        for (j = 0; j < st->nb_index_entries; j++)
            if (st->index_entries[j].pos < pos)
                st->index_entries[out++] = st->index_entries[j];
        st->nb_index_entries = out;
    }
}
