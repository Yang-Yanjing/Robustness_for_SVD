static void find_compressor(char * compressor_name, int len, MOVTrack *track)
{
    AVDictionaryEntry *encoder;
    int xdcam_res =  (track->enc->width == 1280 && track->enc->height == 720)
                  || (track->enc->width == 1440 && track->enc->height == 1080)
                  || (track->enc->width == 1920 && track->enc->height == 1080);
    if (track->mode == MODE_MOV &&
        (encoder = av_dict_get(track->st->metadata, "encoder", NULL, 0))) {
        av_strlcpy(compressor_name, encoder->value, 32);
    } else if (track->enc->codec_id == AV_CODEC_ID_MPEG2VIDEO && xdcam_res) {
        int interlaced = track->enc->field_order > AV_FIELD_PROGRESSIVE;
        AVStream *st = track->st;
        int rate = av_q2d(find_fps(NULL, st));
        av_strlcatf(compressor_name, len, "XDCAM");
        if (track->enc->pix_fmt == AV_PIX_FMT_YUV422P) {
            av_strlcatf(compressor_name, len, " HD422");
        } else if(track->enc->width == 1440) {
            av_strlcatf(compressor_name, len, " HD");
        } else
            av_strlcatf(compressor_name, len, " EX");
        av_strlcatf(compressor_name, len, " %d%c", track->enc->height, interlaced ? 'i' : 'p');
        av_strlcatf(compressor_name, len, "%d", rate * (interlaced + 1));
    }
}
