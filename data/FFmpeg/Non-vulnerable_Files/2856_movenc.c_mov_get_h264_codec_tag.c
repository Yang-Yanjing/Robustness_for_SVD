static int mov_get_h264_codec_tag(AVFormatContext *s, MOVTrack *track)
{
    int tag = track->enc->codec_tag;
    int interlaced = track->enc->field_order > AV_FIELD_PROGRESSIVE;
    AVStream *st = track->st;
    int rate = av_q2d(find_fps(s, st));
    if (!tag)
        tag = MKTAG('a', 'v', 'c', 'i'); 
    if (track->enc->pix_fmt == AV_PIX_FMT_YUV420P10) {
        if (track->enc->width == 960 && track->enc->height == 720) {
            if (!interlaced) {
                if      (rate == 24) tag = MKTAG('a','i','5','p');
                else if (rate == 25) tag = MKTAG('a','i','5','q');
                else if (rate == 30) tag = MKTAG('a','i','5','p');
                else if (rate == 50) tag = MKTAG('a','i','5','q');
                else if (rate == 60) tag = MKTAG('a','i','5','p');
            }
        } else if (track->enc->width == 1440 && track->enc->height == 1080) {
            if (!interlaced) {
                if      (rate == 24) tag = MKTAG('a','i','5','3');
                else if (rate == 25) tag = MKTAG('a','i','5','2');
                else if (rate == 30) tag = MKTAG('a','i','5','3');
            } else {
                if      (rate == 50) tag = MKTAG('a','i','5','5');
                else if (rate == 60) tag = MKTAG('a','i','5','6');
            }
        }
    } else if (track->enc->pix_fmt == AV_PIX_FMT_YUV422P10) {
        if (track->enc->width == 1280 && track->enc->height == 720) {
            if (!interlaced) {
                if      (rate == 24) tag = MKTAG('a','i','1','p');
                else if (rate == 25) tag = MKTAG('a','i','1','q');
                else if (rate == 30) tag = MKTAG('a','i','1','p');
                else if (rate == 50) tag = MKTAG('a','i','1','q');
                else if (rate == 60) tag = MKTAG('a','i','1','p');
            }
        } else if (track->enc->width == 1920 && track->enc->height == 1080) {
            if (!interlaced) {
                if      (rate == 24) tag = MKTAG('a','i','1','3');
                else if (rate == 25) tag = MKTAG('a','i','1','2');
                else if (rate == 30) tag = MKTAG('a','i','1','3');
            } else {
                if      (rate == 25) tag = MKTAG('a','i','1','5');
                else if (rate == 50) tag = MKTAG('a','i','1','5');
                else if (rate == 60) tag = MKTAG('a','i','1','6');
            }
        } else if (   track->enc->width == 4096 && track->enc->height == 2160
                   || track->enc->width == 3840 && track->enc->height == 2160
                   || track->enc->width == 2048 && track->enc->height == 1080) {
            tag = MKTAG('a','i','v','x');
        }
    }
    return tag;
}
