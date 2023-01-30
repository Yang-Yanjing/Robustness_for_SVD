static int mxf_write_tagged_value(AVFormatContext *s, const char* name, const char* value)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    int name_size = mxf_utf16_local_tag_length(name);
    int indirect_value_size = 13 + mxf_utf16_local_tag_length(value);
    if (!name_size || indirect_value_size == 13)
        return 1;
    mxf_write_metadata_key(pb, 0x013f00);
    klv_encode_ber_length(pb, 24 + name_size + indirect_value_size);
    
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, TaggedValue, mxf->tagged_value_count);
    
    mxf_write_local_tag_utf16(pb, 0x5001, name); 
    
    mxf_write_local_tag(pb, indirect_value_size, 0x5003);
    avio_write(pb, mxf_indirect_value_utf16le, 17);
    avio_put_str16le(pb, value);
    mxf->tagged_value_count++;
    return 0;
}
