static void show_format_opts(void)
{
    AVInputFormat *iformat = NULL;
    AVOutputFormat *oformat = NULL;
    printf("@section Generic format AVOptions\n");
    show_opts(avformat_get_class());
    printf("@section Format-specific AVOptions\n");
    while ((iformat = av_iformat_next(iformat))) {
        if (!iformat->priv_class)
            continue;
        printf("@subsection %s AVOptions\n", iformat->priv_class->class_name);
        show_opts(iformat->priv_class);
    }
    while ((oformat = av_oformat_next(oformat))) {
        if (!oformat->priv_class)
            continue;
        printf("@subsection %s AVOptions\n", oformat->priv_class->class_name);
        show_opts(oformat->priv_class);
    }
}
