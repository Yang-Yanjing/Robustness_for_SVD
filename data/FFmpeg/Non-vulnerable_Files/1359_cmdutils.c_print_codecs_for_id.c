static void print_codecs_for_id(enum AVCodecID id, int encoder)
{
    const AVCodec *codec = NULL;
    printf(" (%s: ", encoder ? "encoders" : "decoders");
    while ((codec = next_codec_for_id(id, codec, encoder)))
        printf("%s ", codec->name);
    printf(")");
}
