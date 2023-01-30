static int matroska_parse_flac(AVFormatContext *s,
                               MatroskaTrack *track,
                               int *offset)
{
    AVStream *st = track->stream;
    uint8_t *p = track->codec_priv.data;
    int size   = track->codec_priv.size;
    if (size < 8 + FLAC_STREAMINFO_SIZE || p[4] & 0x7f) {
        av_log(s, AV_LOG_WARNING, "Invalid FLAC private data\n");
        track->codec_priv.size = 0;
        return 0;
    }
    *offset = 8;
    track->codec_priv.size = 8 + FLAC_STREAMINFO_SIZE;
    p    += track->codec_priv.size;
    size -= track->codec_priv.size;
    
    while (size >= 4) {
        int block_last, block_type, block_size;
        flac_parse_block_header(p, &block_last, &block_type, &block_size);
        p    += 4;
        size -= 4;
        if (block_size > size)
            return 0;
        
        if (block_type == FLAC_METADATA_TYPE_VORBIS_COMMENT) {
            AVDictionary *dict = NULL;
            AVDictionaryEntry *chmask;
            ff_vorbis_comment(s, &dict, p, block_size, 0);
            chmask = av_dict_get(dict, "WAVEFORMATEXTENSIBLE_CHANNEL_MASK", NULL, 0);
            if (chmask) {
                uint64_t mask = strtol(chmask->value, NULL, 0);
                if (!mask || mask & ~0x3ffffULL) {
                    av_log(s, AV_LOG_WARNING,
                           "Invalid value of WAVEFORMATEXTENSIBLE_CHANNEL_MASK\n");
                } else
                    st->codec->channel_layout = mask;
            }
            av_dict_free(&dict);
        }
        p    += block_size;
        size -= block_size;
    }
    return 0;
}
