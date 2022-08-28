static int amf_skip_tag(AVIOContext *pb, AMFDataType type)
{
    int nb = -1, ret, parse_name = 1;
    switch (type) {
    case AMF_DATA_TYPE_NUMBER:
        avio_skip(pb, 8);
        break;
    case AMF_DATA_TYPE_BOOL:
        avio_skip(pb, 1);
        break;
    case AMF_DATA_TYPE_STRING:
        avio_skip(pb, avio_rb16(pb));
        break;
    case AMF_DATA_TYPE_ARRAY:
        parse_name = 0;
    case AMF_DATA_TYPE_MIXEDARRAY:
        nb = avio_rb32(pb);
    case AMF_DATA_TYPE_OBJECT:
        while(!pb->eof_reached && (nb-- > 0 || type != AMF_DATA_TYPE_ARRAY)) {
            if (parse_name) {
                int size = avio_rb16(pb);
                if (!size) {
                    avio_skip(pb, 1);
                    break;
                }
                avio_skip(pb, size);
            }
            if ((ret = amf_skip_tag(pb, avio_r8(pb))) < 0)
                return ret;
        }
        break;
    case AMF_DATA_TYPE_NULL:
    case AMF_DATA_TYPE_OBJECT_END:
        break;
    default:
        return AVERROR_INVALIDDATA;
    }
    return 0;
}
