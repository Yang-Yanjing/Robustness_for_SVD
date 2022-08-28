static void mxf_write_identification(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    const char *company = "FFmpeg";
    const char *product = s->oformat != &ff_mxf_opatom_muxer ? "OP1a Muxer" : "OPAtom Muxer";
    const char *version;
    int length;
    mxf_write_metadata_key(pb, 0x013000);
    PRINT_KEY(s, "identification key", pb->buf_ptr - 16);
    version = s->flags & AVFMT_FLAG_BITEXACT ?
        "0.0.0" : AV_STRINGIFY(LIBAVFORMAT_VERSION);
    length = 72 + mxf_utf16_local_tag_length(company) +
                  mxf_utf16_local_tag_length(product) +
                  mxf_utf16_local_tag_length(version);
    klv_encode_ber_length(pb, length);
    
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, Identification, 0);
    PRINT_KEY(s, "identification uid", pb->buf_ptr - 16);
    
    mxf_write_local_tag(pb, 16, 0x3C09);
    mxf_write_uuid(pb, Identification, 1);
    mxf_write_local_tag_utf16(pb, 0x3C01, company); 
    mxf_write_local_tag_utf16(pb, 0x3C02, product); 
    mxf_write_local_tag_utf16(pb, 0x3C04, version); 
    
    mxf_write_local_tag(pb, 16, 0x3C05);
    mxf_write_uuid(pb, Identification, 2);
    
    mxf_write_local_tag(pb, 8, 0x3C06);
    avio_wb64(pb, mxf->timestamp);
}
