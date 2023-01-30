static int get_private_data(struct Track *track, AVCodecContext *codec)
{
    track->codec_private_size = 0;
    track->codec_private      = av_mallocz(codec->extradata_size);
    if (!track->codec_private)
        return AVERROR(ENOMEM);
    track->codec_private_size = codec->extradata_size;
    memcpy(track->codec_private, codec->extradata, codec->extradata_size);
    return 0;
}
