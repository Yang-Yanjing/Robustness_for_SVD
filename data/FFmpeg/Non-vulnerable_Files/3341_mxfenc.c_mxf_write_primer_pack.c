static void mxf_write_primer_pack(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    int local_tag_number, i = 0;
    local_tag_number = FF_ARRAY_ELEMS(mxf_local_tag_batch);
    avio_write(pb, primer_pack_key, 16);
    klv_encode_ber_length(pb, local_tag_number * 18 + 8);
    avio_wb32(pb, local_tag_number); 
    avio_wb32(pb, 18); 
    for (i = 0; i < local_tag_number; i++) {
        avio_wb16(pb, mxf_local_tag_batch[i].local_tag);
        avio_write(pb, mxf_local_tag_batch[i].uid, 16);
    }
}
