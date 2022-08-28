static int mkv_write_stereo_mode(AVFormatContext *s, AVIOContext *pb,
                                 AVStream *st, int mode, int *h_width, int *h_height)
{
    int i;
    int ret = 0;
    AVDictionaryEntry *tag;
    MatroskaVideoStereoModeType format = MATROSKA_VIDEO_STEREOMODE_TYPE_NB;
    *h_width = 1;
    *h_height = 1;
    
    if ((tag = av_dict_get(st->metadata, "stereo_mode", NULL, 0)) ||
        (tag = av_dict_get( s->metadata, "stereo_mode", NULL, 0))) {
        int stereo_mode = atoi(tag->value);
        for (i=0; i<MATROSKA_VIDEO_STEREOMODE_TYPE_NB; i++)
            if (!strcmp(tag->value, ff_matroska_video_stereo_mode[i])){
                stereo_mode = i;
                break;
            }
        if (stereo_mode < MATROSKA_VIDEO_STEREOMODE_TYPE_NB &&
            stereo_mode != 10 && stereo_mode != 12) {
            int ret = ff_mkv_stereo3d_conv(st, stereo_mode);
            if (ret < 0)
                return ret;
        }
    }
    
    for (i = 0; i < st->nb_side_data; i++) {
        AVPacketSideData sd = st->side_data[i];
        if (sd.type == AV_PKT_DATA_STEREO3D) {
            AVStereo3D *stereo = (AVStereo3D *)sd.data;
            switch (stereo->type) {
            case AV_STEREO3D_2D:
                format = MATROSKA_VIDEO_STEREOMODE_TYPE_MONO;
                break;
            case AV_STEREO3D_SIDEBYSIDE:
                format = (stereo->flags & AV_STEREO3D_FLAG_INVERT)
                    ? MATROSKA_VIDEO_STEREOMODE_TYPE_RIGHT_LEFT
                    : MATROSKA_VIDEO_STEREOMODE_TYPE_LEFT_RIGHT;
                *h_width = 2;
                break;
            case AV_STEREO3D_TOPBOTTOM:
                format = MATROSKA_VIDEO_STEREOMODE_TYPE_TOP_BOTTOM;
                if (stereo->flags & AV_STEREO3D_FLAG_INVERT)
                    format--;
                *h_height = 2;
                break;
            case AV_STEREO3D_CHECKERBOARD:
                format = MATROSKA_VIDEO_STEREOMODE_TYPE_CHECKERBOARD_LR;
                if (stereo->flags & AV_STEREO3D_FLAG_INVERT)
                    format--;
                break;
            case AV_STEREO3D_LINES:
                format = MATROSKA_VIDEO_STEREOMODE_TYPE_ROW_INTERLEAVED_LR;
                if (stereo->flags & AV_STEREO3D_FLAG_INVERT)
                    format--;
                *h_height = 2;
                break;
            case AV_STEREO3D_COLUMNS:
                format = MATROSKA_VIDEO_STEREOMODE_TYPE_COL_INTERLEAVED_LR;
                if (stereo->flags & AV_STEREO3D_FLAG_INVERT)
                    format--;
                *h_width = 2;
                break;
            case AV_STEREO3D_FRAMESEQUENCE:
                format = MATROSKA_VIDEO_STEREOMODE_TYPE_BOTH_EYES_BLOCK_LR;
                if (stereo->flags & AV_STEREO3D_FLAG_INVERT)
                    format++;
                break;
            }
            break;
        }
    }
    if (format == MATROSKA_VIDEO_STEREOMODE_TYPE_NB)
        return ret;
    
    if ((mode == MODE_WEBM &&
        format > MATROSKA_VIDEO_STEREOMODE_TYPE_TOP_BOTTOM &&
        format != MATROSKA_VIDEO_STEREOMODE_TYPE_RIGHT_LEFT)
        || format >= MATROSKA_VIDEO_STEREOMODE_TYPE_NB) {
        av_log(s, AV_LOG_ERROR,
               "The specified stereo mode is not valid.\n");
        format = MATROSKA_VIDEO_STEREOMODE_TYPE_NB;
        return AVERROR(EINVAL);
    }
    
    put_ebml_uint(pb, MATROSKA_ID_VIDEOSTEREOMODE, format);
    return ret;
}
