static int mkv_add_seekhead_entry(mkv_seekhead *seekhead, unsigned int elementid, uint64_t filepos)
{
    mkv_seekhead_entry *entries = seekhead->entries;
    
    if (seekhead->max_entries > 0 && seekhead->max_entries <= seekhead->num_entries)
        return -1;
    entries = av_realloc_array(entries, seekhead->num_entries + 1, sizeof(mkv_seekhead_entry));
    if (!entries)
        return AVERROR(ENOMEM);
    seekhead->entries = entries;
    seekhead->entries[seekhead->num_entries].elementid    = elementid;
    seekhead->entries[seekhead->num_entries++].segmentpos = filepos - seekhead->segment_offset;
    return 0;
}
