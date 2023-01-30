static void flv_set_audio_codec(AVFormatContext *s, AVStream *astream,
                                AVCodecContext *acodec, int flv_codecid)
{
    switch (flv_codecid) {
    
    case FLV_CODECID_PCM:
        acodec->codec_id = acodec->bits_per_coded_sample == 8
                           ? AV_CODEC_ID_PCM_U8
#if HAVE_BIGENDIAN
                           : AV_CODEC_ID_PCM_S16BE;
#else
                           : AV_CODEC_ID_PCM_S16LE;
#endif
        break;
    case FLV_CODECID_PCM_LE:
        acodec->codec_id = acodec->bits_per_coded_sample == 8
                           ? AV_CODEC_ID_PCM_U8
                           : AV_CODEC_ID_PCM_S16LE;
        break;
    case FLV_CODECID_AAC:
        acodec->codec_id = AV_CODEC_ID_AAC;
        break;
    case FLV_CODECID_ADPCM:
        acodec->codec_id = AV_CODEC_ID_ADPCM_SWF;
        break;
    case FLV_CODECID_SPEEX:
        acodec->codec_id    = AV_CODEC_ID_SPEEX;
        acodec->sample_rate = 16000;
        break;
    case FLV_CODECID_MP3:
        acodec->codec_id      = AV_CODEC_ID_MP3;
        astream->need_parsing = AVSTREAM_PARSE_FULL;
        break;
    case FLV_CODECID_NELLYMOSER_8KHZ_MONO:
        
        acodec->sample_rate = 8000;
        acodec->codec_id    = AV_CODEC_ID_NELLYMOSER;
        break;
    case FLV_CODECID_NELLYMOSER_16KHZ_MONO:
        acodec->sample_rate = 16000;
        acodec->codec_id    = AV_CODEC_ID_NELLYMOSER;
        break;
    case FLV_CODECID_NELLYMOSER:
        acodec->codec_id = AV_CODEC_ID_NELLYMOSER;
        break;
    case FLV_CODECID_PCM_MULAW:
        acodec->sample_rate = 8000;
        acodec->codec_id    = AV_CODEC_ID_PCM_MULAW;
        break;
    case FLV_CODECID_PCM_ALAW:
        acodec->sample_rate = 8000;
        acodec->codec_id    = AV_CODEC_ID_PCM_ALAW;
        break;
    default:
        avpriv_request_sample(s, "Audio codec (%x)",
               flv_codecid >> FLV_AUDIO_CODECID_OFFSET);
        acodec->codec_tag = flv_codecid >> FLV_AUDIO_CODECID_OFFSET;
    }
}
