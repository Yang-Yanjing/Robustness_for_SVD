const int av_ripemd_size = sizeof(AVRIPEMD);
struct AVRIPEMD *av_ripemd_alloc(void)
{
    return av_mallocz(sizeof(struct AVRIPEMD));
}
