const int av_md5_size = sizeof(AVMD5);
struct AVMD5 *av_md5_alloc(void)
{
    return av_mallocz(sizeof(struct AVMD5));
}
