static int matroska_decode_buffer(uint8_t **buf, int *buf_size,
                                  MatroskaTrack *track)
{
    MatroskaTrackEncoding *encodings = track->encodings.elem;
    uint8_t *data = *buf;
    int isize = *buf_size;
    uint8_t *pkt_data = NULL;
    uint8_t av_unused *newpktdata;
    int pkt_size = isize;
    int result = 0;
    int olen;
    if (pkt_size >= 10000000U)
        return AVERROR_INVALIDDATA;
    switch (encodings[0].compression.algo) {
    case MATROSKA_TRACK_ENCODING_COMP_HEADERSTRIP:
    {
        int header_size = encodings[0].compression.settings.size;
        uint8_t *header = encodings[0].compression.settings.data;
        if (header_size && !header) {
            av_log(NULL, AV_LOG_ERROR, "Compression size but no data in headerstrip\n");
            return -1;
        }
        if (!header_size)
            return 0;
        pkt_size = isize + header_size;
        pkt_data = av_malloc(pkt_size);
        if (!pkt_data)
            return AVERROR(ENOMEM);
        memcpy(pkt_data, header, header_size);
        memcpy(pkt_data + header_size, data, isize);
        break;
    }
#if CONFIG_LZO
    case MATROSKA_TRACK_ENCODING_COMP_LZO:
        do {
            olen       = pkt_size *= 3;
            newpktdata = av_realloc(pkt_data, pkt_size + AV_LZO_OUTPUT_PADDING);
            if (!newpktdata) {
                result = AVERROR(ENOMEM);
                goto failed;
            }
            pkt_data = newpktdata;
            result   = av_lzo1x_decode(pkt_data, &olen, data, &isize);
        } while (result == AV_LZO_OUTPUT_FULL && pkt_size < 10000000);
        if (result) {
            result = AVERROR_INVALIDDATA;
            goto failed;
        }
        pkt_size -= olen;
        break;
#endif
#if CONFIG_ZLIB
    case MATROSKA_TRACK_ENCODING_COMP_ZLIB:
    {
        z_stream zstream = { 0 };
        if (inflateInit(&zstream) != Z_OK)
            return -1;
        zstream.next_in  = data;
        zstream.avail_in = isize;
        do {
            pkt_size  *= 3;
            newpktdata = av_realloc(pkt_data, pkt_size);
            if (!newpktdata) {
                inflateEnd(&zstream);
                result = AVERROR(ENOMEM);
                goto failed;
            }
            pkt_data          = newpktdata;
            zstream.avail_out = pkt_size - zstream.total_out;
            zstream.next_out  = pkt_data + zstream.total_out;
            result = inflate(&zstream, Z_NO_FLUSH);
        } while (result == Z_OK && pkt_size < 10000000);
        pkt_size = zstream.total_out;
        inflateEnd(&zstream);
        if (result != Z_STREAM_END) {
            if (result == Z_MEM_ERROR)
                result = AVERROR(ENOMEM);
            else
                result = AVERROR_INVALIDDATA;
            goto failed;
        }
        break;
    }
#endif
#if CONFIG_BZLIB
    case MATROSKA_TRACK_ENCODING_COMP_BZLIB:
    {
        bz_stream bzstream = { 0 };
        if (BZ2_bzDecompressInit(&bzstream, 0, 0) != BZ_OK)
            return -1;
        bzstream.next_in  = data;
        bzstream.avail_in = isize;
        do {
            pkt_size  *= 3;
            newpktdata = av_realloc(pkt_data, pkt_size);
            if (!newpktdata) {
                BZ2_bzDecompressEnd(&bzstream);
                result = AVERROR(ENOMEM);
                goto failed;
            }
            pkt_data           = newpktdata;
            bzstream.avail_out = pkt_size - bzstream.total_out_lo32;
            bzstream.next_out  = pkt_data + bzstream.total_out_lo32;
            result = BZ2_bzDecompress(&bzstream);
        } while (result == BZ_OK && pkt_size < 10000000);
        pkt_size = bzstream.total_out_lo32;
        BZ2_bzDecompressEnd(&bzstream);
        if (result != BZ_STREAM_END) {
            if (result == BZ_MEM_ERROR)
                result = AVERROR(ENOMEM);
            else
                result = AVERROR_INVALIDDATA;
            goto failed;
        }
        break;
    }
#endif
    default:
        return AVERROR_INVALIDDATA;
    }
    *buf      = pkt_data;
    *buf_size = pkt_size;
    return 0;
failed:
    av_free(pkt_data);
    return result;
}
