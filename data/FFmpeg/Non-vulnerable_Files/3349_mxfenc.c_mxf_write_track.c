static void mxf_write_track(AVFormatContext *s, AVStream *st, enum MXFMetadataSetType type)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    MXFStreamContext *sc = st->priv_data;
    mxf_write_metadata_key(pb, 0x013b00);
    PRINT_KEY(s, "track key", pb->buf_ptr - 16);
    klv_encode_ber_length(pb, 80);
    
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, type == MaterialPackage ? Track : Track + TypeBottom, st->index);
    PRINT_KEY(s, "track uid", pb->buf_ptr - 16);
    
    mxf_write_local_tag(pb, 4, 0x4801);
    avio_wb32(pb, st->index+2);
    
    mxf_write_local_tag(pb, 4, 0x4804);
    if (type == MaterialPackage)
        avio_wb32(pb, 0); 
    else
        avio_write(pb, sc->track_essence_element_key + 12, 4);
    mxf_write_local_tag(pb, 8, 0x4B01);
    if (st == mxf->timecode_track && s->oformat == &ff_mxf_opatom_muxer){
        avio_wb32(pb, mxf->tc.rate.num);
        avio_wb32(pb, mxf->tc.rate.den);
    } else {
        avio_wb32(pb, mxf->time_base.den);
        avio_wb32(pb, mxf->time_base.num);
    }
    
    mxf_write_local_tag(pb, 8, 0x4B02);
    avio_wb64(pb, 0);
    
    mxf_write_local_tag(pb, 16, 0x4803);
    mxf_write_uuid(pb, type == MaterialPackage ? Sequence: Sequence + TypeBottom, st->index);
}
