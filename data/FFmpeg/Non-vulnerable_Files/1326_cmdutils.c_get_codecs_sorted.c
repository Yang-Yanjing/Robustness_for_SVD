static unsigned get_codecs_sorted(const AVCodecDescriptor ***rcodecs)
{
    const AVCodecDescriptor *desc = NULL;
    const AVCodecDescriptor **codecs;
    unsigned nb_codecs = 0, i = 0;
    while ((desc = avcodec_descriptor_next(desc)))
        nb_codecs++;
    if (!(codecs = av_calloc(nb_codecs, sizeof(*codecs)))) {
        av_log(NULL, AV_LOG_ERROR, "Out of memory\n");
        exit_program(1);
    }
    desc = NULL;
    while ((desc = avcodec_descriptor_next(desc)))
        codecs[i++] = desc;
    av_assert0(i == nb_codecs);
    qsort(codecs, nb_codecs, sizeof(*codecs), compare_codec_desc);
    *rcodecs = codecs;
    return nb_codecs;
}
