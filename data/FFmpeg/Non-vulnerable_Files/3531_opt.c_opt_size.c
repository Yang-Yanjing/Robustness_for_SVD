static int opt_size(enum AVOptionType type)
{
    switch(type) {
    case AV_OPT_TYPE_INT:
    case AV_OPT_TYPE_FLAGS:     return sizeof(int);
    case AV_OPT_TYPE_DURATION:
    case AV_OPT_TYPE_CHANNEL_LAYOUT:
    case AV_OPT_TYPE_INT64:     return sizeof(int64_t);
    case AV_OPT_TYPE_DOUBLE:    return sizeof(double);
    case AV_OPT_TYPE_FLOAT:     return sizeof(float);
    case AV_OPT_TYPE_STRING:    return sizeof(uint8_t*);
    case AV_OPT_TYPE_VIDEO_RATE:
    case AV_OPT_TYPE_RATIONAL:  return sizeof(AVRational);
    case AV_OPT_TYPE_BINARY:    return sizeof(uint8_t*) + sizeof(int);
    case AV_OPT_TYPE_IMAGE_SIZE:return sizeof(int[2]);
    case AV_OPT_TYPE_PIXEL_FMT: return sizeof(enum AVPixelFormat);
    case AV_OPT_TYPE_SAMPLE_FMT:return sizeof(enum AVSampleFormat);
    case AV_OPT_TYPE_COLOR:     return 4;
    }
    return 0;
}
