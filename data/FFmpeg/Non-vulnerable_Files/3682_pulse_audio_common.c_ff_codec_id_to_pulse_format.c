pa_sample_format_t av_cold ff_codec_id_to_pulse_format(enum AVCodecID codec_id)
{
    switch (codec_id) {
    case AV_CODEC_ID_PCM_U8:    return PA_SAMPLE_U8;
    case AV_CODEC_ID_PCM_ALAW:  return PA_SAMPLE_ALAW;
    case AV_CODEC_ID_PCM_MULAW: return PA_SAMPLE_ULAW;
    case AV_CODEC_ID_PCM_S16LE: return PA_SAMPLE_S16LE;
    case AV_CODEC_ID_PCM_S16BE: return PA_SAMPLE_S16BE;
    case AV_CODEC_ID_PCM_F32LE: return PA_SAMPLE_FLOAT32LE;
    case AV_CODEC_ID_PCM_F32BE: return PA_SAMPLE_FLOAT32BE;
    case AV_CODEC_ID_PCM_S32LE: return PA_SAMPLE_S32LE;
    case AV_CODEC_ID_PCM_S32BE: return PA_SAMPLE_S32BE;
    case AV_CODEC_ID_PCM_S24LE: return PA_SAMPLE_S24LE;
    case AV_CODEC_ID_PCM_S24BE: return PA_SAMPLE_S24BE;
    default:                    return PA_SAMPLE_INVALID;
    }
}
