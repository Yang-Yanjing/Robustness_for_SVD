static enum AVCodecID vfw_codecid(DWORD biCompression)
{
    switch(biCompression) {
    case MKTAG('d', 'v', 's', 'd'):
        return AV_CODEC_ID_DVVIDEO;
    case MKTAG('M', 'J', 'P', 'G'):
    case MKTAG('m', 'j', 'p', 'g'):
        return AV_CODEC_ID_MJPEG;
    }
    return AV_CODEC_ID_NONE;
}
