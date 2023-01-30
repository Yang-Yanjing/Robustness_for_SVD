static mkv_seekhead *mkv_start_seekhead(AVIOContext *pb, int64_t segment_offset,
                                        int numelements)
{
    mkv_seekhead *new_seekhead = av_mallocz(sizeof(mkv_seekhead));
    if (!new_seekhead)
        return NULL;
    new_seekhead->segment_offset = segment_offset;
    if (numelements > 0) {
        new_seekhead->filepos = avio_tell(pb);
        
        
        
        new_seekhead->reserved_size = numelements * MAX_SEEKENTRY_SIZE + 13;
        new_seekhead->max_entries   = numelements;
        put_ebml_void(pb, new_seekhead->reserved_size);
    }
    return new_seekhead;
}
