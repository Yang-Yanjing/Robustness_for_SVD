static int mov_get_mpeg2_xdcam_codec_tag(AVFormatContext *s, MOVTrack *track)
{
    int tag = track->enc->codec_tag;
    int interlaced = track->enc->field_order > AV_FIELD_PROGRESSIVE;
    AVStream *st = track->st;
    int rate = av_q2d(find_fps(s, st));
    if (!tag)
        tag = MKTAG('m', '2', 'v', '1'); 
    if (track->enc->pix_fmt == AV_PIX_FMT_YUV420P) {
        if (track->enc->width == 1280 && track->enc->height == 720) {
            if (!interlaced) {
                if      (rate == 24) tag = MKTAG('x','d','v','4');
                else if (rate == 25) tag = MKTAG('x','d','v','5');
                else if (rate == 30) tag = MKTAG('x','d','v','1');
                else if (rate == 50) tag = MKTAG('x','d','v','a');
                else if (rate == 60) tag = MKTAG('x','d','v','9');
            }
        } else if (track->enc->width == 1440 && track->enc->height == 1080) {
            if (!interlaced) {
                if      (rate == 24) tag = MKTAG('x','d','v','6');
                else if (rate == 25) tag = MKTAG('x','d','v','7');
                else if (rate == 30) tag = MKTAG('x','d','v','8');
            } else {
                if      (rate == 25) tag = MKTAG('x','d','v','3');
                else if (rate == 30) tag = MKTAG('x','d','v','2');
            }
        } else if (track->enc->width == 1920 && track->enc->height == 1080) {
            if (!interlaced) {
                if      (rate == 24) tag = MKTAG('x','d','v','d');
                else if (rate == 25) tag = MKTAG('x','d','v','e');
                else if (rate == 30) tag = MKTAG('x','d','v','f');
            } else {
                if      (rate == 25) tag = MKTAG('x','d','v','c');
                else if (rate == 30) tag = MKTAG('x','d','v','b');
            }
        }
    } else if (track->enc->pix_fmt == AV_PIX_FMT_YUV422P) {
        if (track->enc->width == 1280 && track->enc->height == 720) {
            if (!interlaced) {
                if      (rate == 24) tag = MKTAG('x','d','5','4');
                else if (rate == 25) tag = MKTAG('x','d','5','5');
                else if (rate == 30) tag = MKTAG('x','d','5','1');
                else if (rate == 50) tag = MKTAG('x','d','5','a');
                else if (rate == 60) tag = MKTAG('x','d','5','9');
            }
        } else if (track->enc->width == 1920 && track->enc->height == 1080) {
            if (!interlaced) {
                if      (rate == 24) tag = MKTAG('x','d','5','d');
                else if (rate == 25) tag = MKTAG('x','d','5','e');
                else if (rate == 30) tag = MKTAG('x','d','5','f');
            } else {
                if      (rate == 25) tag = MKTAG('x','d','5','c');
                else if (rate == 30) tag = MKTAG('x','d','5','b');
            }
        }
    }
    return tag;
}
