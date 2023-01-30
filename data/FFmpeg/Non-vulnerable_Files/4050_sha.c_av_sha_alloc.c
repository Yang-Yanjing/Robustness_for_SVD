const int av_sha_size = sizeof(AVSHA);
struct AVSHA *av_sha_alloc(void)
{
    return av_mallocz(sizeof(struct AVSHA));
}
