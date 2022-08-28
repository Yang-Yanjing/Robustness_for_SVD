static int encode_intervals(struct sbg_script *s, AVCodecContext *avc,
                            struct ws_intervals *inter)
{
    int i, edata_size = 4;
    uint8_t *edata;
    for (i = 0; i < inter->nb_inter; i++) {
        edata_size += inter->inter[i].type == WS_SINE  ? 44 :
                      inter->inter[i].type == WS_NOISE ? 32 : 0;
        if (edata_size < 0)
            return AVERROR(ENOMEM);
    }
    if (ff_alloc_extradata(avc, edata_size))
        return AVERROR(ENOMEM);
    edata = avc->extradata;
#define ADD_EDATA32(v) do { AV_WL32(edata, (v)); edata += 4; } while(0)
#define ADD_EDATA64(v) do { AV_WL64(edata, (v)); edata += 8; } while(0)
    ADD_EDATA32(inter->nb_inter);
    for (i = 0; i < inter->nb_inter; i++) {
        ADD_EDATA64(inter->inter[i].ts1);
        ADD_EDATA64(inter->inter[i].ts2);
        ADD_EDATA32(inter->inter[i].type);
        ADD_EDATA32(inter->inter[i].channels);
        switch (inter->inter[i].type) {
            case WS_SINE:
                ADD_EDATA32(inter->inter[i].f1);
                ADD_EDATA32(inter->inter[i].f2);
                ADD_EDATA32(inter->inter[i].a1);
                ADD_EDATA32(inter->inter[i].a2);
                ADD_EDATA32(inter->inter[i].phi);
                break;
            case WS_NOISE:
                ADD_EDATA32(inter->inter[i].a1);
                ADD_EDATA32(inter->inter[i].a2);
                break;
        }
    }
    if (edata != avc->extradata + edata_size)
        return AVERROR_BUG;
    return 0;
}
