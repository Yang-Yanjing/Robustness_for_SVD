static int ffserver_opt_preset(const char *arg, int type, FFServerConfig *config)
{
    FILE *f=NULL;
    char filename[1000], tmp[1000], tmp2[1000], line[1000];
    int ret = 0;
    AVCodecContext *avctx;
    const AVCodec *codec;
    switch(type) {
    case AV_OPT_FLAG_AUDIO_PARAM:
        avctx = config->dummy_actx;
        break;
    case AV_OPT_FLAG_VIDEO_PARAM:
        avctx = config->dummy_vctx;
        break;
    default:
        av_assert0(0);
    }
    codec = avcodec_find_encoder(avctx->codec_id);
    if (!(f = get_preset_file(filename, sizeof(filename), arg, 0,
                              codec ? codec->name : NULL))) {
        av_log(NULL, AV_LOG_ERROR, "File for preset '%s' not found\n", arg);
        return AVERROR(EINVAL);
    }
    while(!feof(f)){
        int e= fscanf(f, "%999[^\n]\n", line) - 1;
        if(line[0] == '#' && !e)
            continue;
        e|= sscanf(line, "%999[^=]=%999[^\n]\n", tmp, tmp2) - 2;
        if(e){
            av_log(NULL, AV_LOG_ERROR, "%s: Invalid syntax: '%s'\n", filename,
                   line);
            ret = AVERROR(EINVAL);
            break;
        }
        if (!strcmp(tmp, "acodec") && avctx->codec_type == AVMEDIA_TYPE_AUDIO ||
            !strcmp(tmp, "vcodec") && avctx->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            if (ffserver_set_codec(avctx, tmp2, config) < 0)
                break;
        } else if (!strcmp(tmp, "scodec")) {
            av_log(NULL, AV_LOG_ERROR, "Subtitles preset found.\n");
            ret = AVERROR(EINVAL);
            break;
        } else if (ffserver_save_avoption(tmp, tmp2, type, config) < 0)
            break;
    }
    fclose(f);
    return ret;
}
