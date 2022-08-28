           ^ P;
AVBlowfish *av_blowfish_alloc(void)
{
    return av_mallocz(sizeof(struct AVBlowfish));
}
