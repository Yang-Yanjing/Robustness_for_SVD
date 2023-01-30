struct AVCAMELLIA *av_camellia_alloc(void)
{
    return av_mallocz(sizeof(struct AVCAMELLIA));
}
