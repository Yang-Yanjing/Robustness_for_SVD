int show_codecs(void *optctx, const char *opt, const char *arg)
{
    const AVCodecDescriptor **codecs;
    unsigned i, nb_codecs = get_codecs_sorted(&codecs);
    printf("Codecs:\n"
           " D..... = Decoding supported\n"
           " .E.... = Encoding supported\n"
           " ..V... = Video codec\n"
           " ..A... = Audio codec\n"
           " ..S... = Subtitle codec\n"
           " ...I.. = Intra frame-only codec\n"
           " ....L. = Lossy compression\n"
           " .....S = Lossless compression\n"
           " -------\n");
    for (i = 0; i < nb_codecs; i++) {
        const AVCodecDescriptor *desc = codecs[i];
        const AVCodec *codec = NULL;
        if (strstr(desc->name, "_deprecated"))
            continue;
        printf(" ");
        printf(avcodec_find_decoder(desc->id) ? "D" : ".");
        printf(avcodec_find_encoder(desc->id) ? "E" : ".");
        printf("%c", get_media_type_char(desc->type));
        printf((desc->props & AV_CODEC_PROP_INTRA_ONLY) ? "I" : ".");
        printf((desc->props & AV_CODEC_PROP_LOSSY)      ? "L" : ".");
        printf((desc->props & AV_CODEC_PROP_LOSSLESS)   ? "S" : ".");
        printf(" %-20s %s", desc->name, desc->long_name ? desc->long_name : "");
        

        while ((codec = next_codec_for_id(desc->id, codec, 0))) {
            if (strcmp(codec->name, desc->name)) {
                print_codecs_for_id(desc->id, 0);
                break;
            }
        }
        codec = NULL;
        while ((codec = next_codec_for_id(desc->id, codec, 1))) {
            if (strcmp(codec->name, desc->name)) {
                print_codecs_for_id(desc->id, 1);
                break;
            }
        }
        printf("\n");
    }
    av_free(codecs);
    return 0;
}
