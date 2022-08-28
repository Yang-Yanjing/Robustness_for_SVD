static int64_t mkv_write_seekhead(AVIOContext *pb, MatroskaMuxContext *mkv)
{
    mkv_seekhead *seekhead = mkv->main_seekhead;
    ebml_master metaseek, seekentry;
    int64_t currentpos;
    int i;
    currentpos = avio_tell(pb);
    if (seekhead->reserved_size > 0) {
        if (avio_seek(pb, seekhead->filepos, SEEK_SET) < 0) {
            currentpos = -1;
            goto fail;
        }
    }
    metaseek = start_ebml_master(pb, MATROSKA_ID_SEEKHEAD, seekhead->reserved_size);
    for (i = 0; i < seekhead->num_entries; i++) {
        mkv_seekhead_entry *entry = &seekhead->entries[i];
        seekentry = start_ebml_master(pb, MATROSKA_ID_SEEKENTRY, MAX_SEEKENTRY_SIZE);
        put_ebml_id(pb, MATROSKA_ID_SEEKID);
        put_ebml_num(pb, ebml_id_size(entry->elementid), 0);
        put_ebml_id(pb, entry->elementid);
        put_ebml_uint(pb, MATROSKA_ID_SEEKPOSITION, entry->segmentpos);
        end_ebml_master(pb, seekentry);
    }
    end_ebml_master(pb, metaseek);
    if (seekhead->reserved_size > 0) {
        uint64_t remaining = seekhead->filepos + seekhead->reserved_size - avio_tell(pb);
        put_ebml_void(pb, remaining);
        avio_seek(pb, currentpos, SEEK_SET);
        currentpos = seekhead->filepos;
    }
fail:
    av_freep(&mkv->main_seekhead->entries);
    av_freep(&mkv->main_seekhead);
    return currentpos;
}
