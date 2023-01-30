static enum AVPixelFormat vfw_pixfmt(DWORD biCompression, WORD biBitCount)
{
    switch(biCompression) {
    case MKTAG('U', 'Y', 'V', 'Y'):
        return AV_PIX_FMT_UYVY422;
    case MKTAG('Y', 'U', 'Y', '2'):
        return AV_PIX_FMT_YUYV422;
    case MKTAG('I', '4', '2', '0'):
        return AV_PIX_FMT_YUV420P;
    case BI_RGB:
        switch(biBitCount) { 
            case 1:
                return AV_PIX_FMT_MONOWHITE;
            case 4:
                return AV_PIX_FMT_RGB4;
            case 8:
                return AV_PIX_FMT_RGB8;
            case 16:
                return AV_PIX_FMT_RGB555;
            case 24:
                return AV_PIX_FMT_BGR24;
            case 32:
                return AV_PIX_FMT_RGB32;
        }
    }
    return AV_PIX_FMT_NONE;
}
