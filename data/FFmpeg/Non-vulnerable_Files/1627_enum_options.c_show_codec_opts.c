static void show_codec_opts(void)
{
    AVCodec *c = NULL;
    printf("@section Generic codec AVOptions\n");
    show_opts(avcodec_get_class());
    printf("@section Codec-specific AVOptions\n");
    while ((c = av_codec_next(c))) {
        if (!c->priv_class)
            continue;
        printf("@subsection %s AVOptions\n", c->priv_class->class_name);
        show_opts(c->priv_class);
    }
}
