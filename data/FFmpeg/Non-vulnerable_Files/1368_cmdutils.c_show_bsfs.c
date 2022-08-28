int show_bsfs(void *optctx, const char *opt, const char *arg)
{
    const AVBitStreamFilter *bsf = NULL;
    void *opaque = NULL;
    printf("Bitstream filters:\n");
    while ((bsf = av_bsf_next(&opaque)))
        printf("%s\n", bsf->name);
    printf("\n");
    return 0;
}
