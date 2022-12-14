static void mxf_write_preface(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    mxf_write_metadata_key(pb, 0x012f00);
    PRINT_KEY(s, "preface key", pb->buf_ptr - 16);
    klv_encode_ber_length(pb, 130 + 16LL * DESCRIPTOR_COUNT(mxf->essence_container_count));
    
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, Preface, 0);
    PRINT_KEY(s, "preface uid", pb->buf_ptr - 16);
    
    mxf_write_local_tag(pb, 8, 0x3B02);
    avio_wb64(pb, mxf->timestamp);
    
    mxf_write_local_tag(pb, 2, 0x3B05);
    avio_wb16(pb, 258); 
    
    mxf_write_local_tag(pb, 16 + 8, 0x3B06);
    mxf_write_refs_count(pb, 1);
    mxf_write_uuid(pb, Identification, 0);
    
    mxf_write_local_tag(pb, 16, 0x3B03);
    mxf_write_uuid(pb, ContentStorage, 0);
    
    mxf_write_local_tag(pb, 16, 0x3B09);
    if (s->oformat == &ff_mxf_opatom_muxer)
        avio_write(pb, opatom_ul, 16);
    else
        avio_write(pb, op1a_ul, 16);
    
    mxf_write_local_tag(pb, 8 + 16LL * DESCRIPTOR_COUNT(mxf->essence_container_count), 0x3B0A);
    mxf_write_essence_container_refs(s);
    
    mxf_write_local_tag(pb, 8, 0x3B0B);
    avio_wb64(pb, 0);
}
