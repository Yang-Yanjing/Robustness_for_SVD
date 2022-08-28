static int read_header(AVFormatContext *ctx)
{
    al_data *ad = ctx->priv_data;
    static const ALCenum sample_formats[2][2] = {
        { AL_FORMAT_MONO8,  AL_FORMAT_STEREO8  },
        { AL_FORMAT_MONO16, AL_FORMAT_STEREO16 }
    };
    int error = 0;
    const char *error_msg;
    AVStream *st = NULL;
    AVCodecContext *codec = NULL;
    if (ad->list_devices) {
        print_al_capture_devices(ctx);
        return AVERROR_EXIT;
    }
    ad->sample_format = sample_formats[ad->sample_size/8-1][ad->channels-1];
    
    ad->device =
        alcCaptureOpenDevice(ctx->filename[0] ? ctx->filename : NULL,
                             ad->sample_rate,
                             ad->sample_format,
                             ad->sample_rate); 
    if (error = al_get_error(ad->device, &error_msg)) goto fail;
    
    if (!(st = avformat_new_stream(ctx, NULL))) {
        error = AVERROR(ENOMEM);
        goto fail;
    }
    
    avpriv_set_pts_info(st, 64, 1, 1000000);
    
    codec = st->codec;
    codec->codec_type = AVMEDIA_TYPE_AUDIO;
    codec->sample_rate = ad->sample_rate;
    codec->channels = get_al_format_info(ad->sample_format)->channels;
    codec->codec_id = get_al_format_info(ad->sample_format)->codec_id;
    
    ad->sample_step = (av_get_bits_per_sample(get_al_format_info(ad->sample_format)->codec_id) *
                       get_al_format_info(ad->sample_format)->channels) / 8;
    
    alcCaptureStart(ad->device);
    return 0;
fail:
    
    if (ad->device)
        alcCaptureCloseDevice(ad->device);
    if (error_msg)
        av_log(ctx, AV_LOG_ERROR, "Cannot open device: %s\n", error_msg);
    return error;
}
