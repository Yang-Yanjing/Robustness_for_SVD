void av_opt_set_defaults2(void *s, int mask, int flags)
{
    const AVOption *opt = NULL;
    while ((opt = av_opt_next(s, opt))) {
        void *dst = ((uint8_t*)s) + opt->offset;
        if ((opt->flags & mask) != flags)
            continue;
        if (opt->flags & AV_OPT_FLAG_READONLY)
            continue;
        switch (opt->type) {
            case AV_OPT_TYPE_CONST:
                
            break;
            case AV_OPT_TYPE_FLAGS:
            case AV_OPT_TYPE_INT:
            case AV_OPT_TYPE_INT64:
            case AV_OPT_TYPE_DURATION:
            case AV_OPT_TYPE_CHANNEL_LAYOUT:
                write_number(s, opt, dst, 1, 1, opt->default_val.i64);
            break;
            case AV_OPT_TYPE_DOUBLE:
            case AV_OPT_TYPE_FLOAT: {
                double val;
                val = opt->default_val.dbl;
                write_number(s, opt, dst, val, 1, 1);
            }
            break;
            case AV_OPT_TYPE_RATIONAL: {
                AVRational val;
                val = av_d2q(opt->default_val.dbl, INT_MAX);
                write_number(s, opt, dst, 1, val.den, val.num);
            }
            break;
            case AV_OPT_TYPE_COLOR:
                set_string_color(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_STRING:
                set_string(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_IMAGE_SIZE:
                set_string_image_size(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_VIDEO_RATE:
                set_string_video_rate(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_PIXEL_FMT:
                write_number(s, opt, dst, 1, 1, opt->default_val.i64);
                break;
            case AV_OPT_TYPE_SAMPLE_FMT:
                write_number(s, opt, dst, 1, 1, opt->default_val.i64);
                break;
            case AV_OPT_TYPE_BINARY:
                set_string_binary(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_DICT:
                
            break;
            default:
                av_log(s, AV_LOG_DEBUG, "AVOption type %d of option %s not implemented yet\n", opt->type, opt->name);
        }
    }
}
