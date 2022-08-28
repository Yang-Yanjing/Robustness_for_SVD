const int av_aes_size= sizeof(AVAES);
struct AVAES *av_aes_alloc(void)
{
    return av_mallocz(sizeof(struct AVAES));
}
