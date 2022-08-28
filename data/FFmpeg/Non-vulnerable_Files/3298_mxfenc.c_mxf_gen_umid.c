static void mxf_gen_umid(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    uint32_t seed = av_get_random_seed();
    uint64_t umid = seed + 0x5294713400000000LL;
    AV_WB64(mxf->umid  , umid);
    AV_WB64(mxf->umid+8, umid>>8);
    mxf->instance_number = seed & 0xFFFFFF;
}
