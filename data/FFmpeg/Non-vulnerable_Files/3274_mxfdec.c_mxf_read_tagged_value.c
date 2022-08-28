static int mxf_read_tagged_value(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFTaggedValue *tagged_value = arg;
    switch (tag){
    case 0x5001:
        return mxf_read_utf16be_string(pb, size, &tagged_value->name);
    case 0x5003:
        return mxf_read_indirect_value(tagged_value, pb, size);
    }
    return 0;
}
