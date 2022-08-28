int ff_fill_rgba_map(uint8_t *rgba_map, enum AVPixelFormat pix_fmt)
{
    switch (pix_fmt) {
    case AV_PIX_FMT_0RGB:
    case AV_PIX_FMT_ARGB:  rgba_map[ALPHA] = 0; rgba_map[RED  ] = 1; rgba_map[GREEN] = 2; rgba_map[BLUE ] = 3; break;
    case AV_PIX_FMT_0BGR:
    case AV_PIX_FMT_ABGR:  rgba_map[ALPHA] = 0; rgba_map[BLUE ] = 1; rgba_map[GREEN] = 2; rgba_map[RED  ] = 3; break;
    case AV_PIX_FMT_RGB48LE:
    case AV_PIX_FMT_RGB48BE:
    case AV_PIX_FMT_RGBA64BE:
    case AV_PIX_FMT_RGBA64LE:
    case AV_PIX_FMT_RGB0:
    case AV_PIX_FMT_RGBA:
    case AV_PIX_FMT_RGB24: rgba_map[RED  ] = 0; rgba_map[GREEN] = 1; rgba_map[BLUE ] = 2; rgba_map[ALPHA] = 3; break;
    case AV_PIX_FMT_BGR48LE:
    case AV_PIX_FMT_BGR48BE:
    case AV_PIX_FMT_BGRA64BE:
    case AV_PIX_FMT_BGRA64LE:
    case AV_PIX_FMT_BGRA:
    case AV_PIX_FMT_BGR0:
    case AV_PIX_FMT_BGR24: rgba_map[BLUE ] = 0; rgba_map[GREEN] = 1; rgba_map[RED  ] = 2; rgba_map[ALPHA] = 3; break;
    case AV_PIX_FMT_GBRAP:
    case AV_PIX_FMT_GBRP:  rgba_map[GREEN] = 0; rgba_map[BLUE ] = 1; rgba_map[RED  ] = 2; rgba_map[ALPHA] = 3; break;
    default:                    
        return AVERROR(EINVAL);
    }
    return 0;
}
