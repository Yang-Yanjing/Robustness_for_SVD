const int av_sha512_size = sizeof(AVSHA512);
struct AVSHA512 *av_sha512_alloc(void)
{
    return av_mallocz(sizeof(struct AVSHA512));
}
