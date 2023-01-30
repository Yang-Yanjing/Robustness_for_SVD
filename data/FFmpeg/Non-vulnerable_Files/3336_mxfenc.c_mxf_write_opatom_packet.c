static int mxf_write_opatom_packet(AVFormatContext *s, AVPacket *pkt, MXFIndexEntry *ie)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    int err;
    if (!mxf->header_written) {
        if ((err = mxf_write_partition(s, 0, 0, header_open_partition_key, 1)) < 0)
            return err;
        mxf_write_klv_fill(s);
        if ((err = mxf_write_opatom_body_partition(s)) < 0)
            return err;
        mxf->header_written = 1;
    }
    if (!mxf->edit_unit_byte_count) {
        mxf->index_entries[mxf->edit_units_count].offset = mxf->body_offset;
        mxf->index_entries[mxf->edit_units_count].flags = ie->flags;
        mxf->index_entries[mxf->edit_units_count].temporal_ref = ie->temporal_ref;
    }
    mxf->edit_units_count++;
    avio_write(pb, pkt->data, pkt->size);
    mxf->body_offset += pkt->size;
    avio_flush(pb);
    return 0;
}
