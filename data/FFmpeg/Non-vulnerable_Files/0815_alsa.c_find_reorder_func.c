static av_cold int find_reorder_func(AlsaData *s, int codec_id, uint64_t layout, int out)
{
    int format;
    
    if (!out)
        return AVERROR(ENOSYS);
    
    if (layout == AV_CH_LAYOUT_QUAD || layout == AV_CH_LAYOUT_2_2)
        return 0;
    switch (codec_id) {
    case AV_CODEC_ID_PCM_S8:
    case AV_CODEC_ID_PCM_U8:
    case AV_CODEC_ID_PCM_ALAW:
    case AV_CODEC_ID_PCM_MULAW: format = FORMAT_I8;  break;
    case AV_CODEC_ID_PCM_S16LE:
    case AV_CODEC_ID_PCM_S16BE:
    case AV_CODEC_ID_PCM_U16LE:
    case AV_CODEC_ID_PCM_U16BE: format = FORMAT_I16; break;
    case AV_CODEC_ID_PCM_S32LE:
    case AV_CODEC_ID_PCM_S32BE:
    case AV_CODEC_ID_PCM_U32LE:
    case AV_CODEC_ID_PCM_U32BE: format = FORMAT_I32; break;
    case AV_CODEC_ID_PCM_F32LE:
    case AV_CODEC_ID_PCM_F32BE: format = FORMAT_F32; break;
    default:                 return AVERROR(ENOSYS);
    }
    if      (layout == AV_CH_LAYOUT_5POINT0_BACK || layout == AV_CH_LAYOUT_5POINT0)
        PICK_REORDER(50)
    else if (layout == AV_CH_LAYOUT_5POINT1_BACK || layout == AV_CH_LAYOUT_5POINT1)
        PICK_REORDER(51)
    else if (layout == AV_CH_LAYOUT_7POINT1)
        PICK_REORDER(71)
    return s->reorder_func ? 0 : AVERROR(ENOSYS);
}
