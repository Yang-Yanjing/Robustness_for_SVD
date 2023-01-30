static const AVCodec *next_codec_for_id(enum AVCodecID id, const AVCodec *prev,
                                        int encoder)
{
    while ((prev = av_codec_next(prev))) {
        if (prev->id == id &&
            (encoder ? av_codec_is_encoder(prev) : av_codec_is_decoder(prev)))
            return prev;
    }
    return NULL;
}
