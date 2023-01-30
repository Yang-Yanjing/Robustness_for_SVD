static av_cold snd_pcm_format_t codec_id_to_pcm_format(int codec_id)
{
    switch(codec_id) {
        case AV_CODEC_ID_PCM_F64LE: return SND_PCM_FORMAT_FLOAT64_LE;
        case AV_CODEC_ID_PCM_F64BE: return SND_PCM_FORMAT_FLOAT64_BE;
        case AV_CODEC_ID_PCM_F32LE: return SND_PCM_FORMAT_FLOAT_LE;
        case AV_CODEC_ID_PCM_F32BE: return SND_PCM_FORMAT_FLOAT_BE;
        case AV_CODEC_ID_PCM_S32LE: return SND_PCM_FORMAT_S32_LE;
        case AV_CODEC_ID_PCM_S32BE: return SND_PCM_FORMAT_S32_BE;
        case AV_CODEC_ID_PCM_U32LE: return SND_PCM_FORMAT_U32_LE;
        case AV_CODEC_ID_PCM_U32BE: return SND_PCM_FORMAT_U32_BE;
        case AV_CODEC_ID_PCM_S24LE: return SND_PCM_FORMAT_S24_3LE;
        case AV_CODEC_ID_PCM_S24BE: return SND_PCM_FORMAT_S24_3BE;
        case AV_CODEC_ID_PCM_U24LE: return SND_PCM_FORMAT_U24_3LE;
        case AV_CODEC_ID_PCM_U24BE: return SND_PCM_FORMAT_U24_3BE;
        case AV_CODEC_ID_PCM_S16LE: return SND_PCM_FORMAT_S16_LE;
        case AV_CODEC_ID_PCM_S16BE: return SND_PCM_FORMAT_S16_BE;
        case AV_CODEC_ID_PCM_U16LE: return SND_PCM_FORMAT_U16_LE;
        case AV_CODEC_ID_PCM_U16BE: return SND_PCM_FORMAT_U16_BE;
        case AV_CODEC_ID_PCM_S8:    return SND_PCM_FORMAT_S8;
        case AV_CODEC_ID_PCM_U8:    return SND_PCM_FORMAT_U8;
        case AV_CODEC_ID_PCM_MULAW: return SND_PCM_FORMAT_MU_LAW;
        case AV_CODEC_ID_PCM_ALAW:  return SND_PCM_FORMAT_A_LAW;
        default:                 return SND_PCM_FORMAT_UNKNOWN;
    }
}
