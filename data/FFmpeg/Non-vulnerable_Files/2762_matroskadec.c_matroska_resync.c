static int matroska_resync(MatroskaDemuxContext *matroska, int64_t last_pos)
{
    AVIOContext *pb = matroska->ctx->pb;
    uint32_t id;
    matroska->current_id = 0;
    matroska->num_levels = 0;
    
    if (avio_seek(pb, last_pos + 1, SEEK_SET) < 0)
        goto eof;
    id = avio_rb32(pb);
    
    while (!avio_feof(pb)) {
        if (id == MATROSKA_ID_INFO     || id == MATROSKA_ID_TRACKS      ||
            id == MATROSKA_ID_CUES     || id == MATROSKA_ID_TAGS        ||
            id == MATROSKA_ID_SEEKHEAD || id == MATROSKA_ID_ATTACHMENTS ||
            id == MATROSKA_ID_CLUSTER  || id == MATROSKA_ID_CHAPTERS) {
            matroska->current_id = id;
            return 0;
        }
        id = (id << 8) | avio_r8(pb);
    }
eof:
    matroska->done = 1;
    return AVERROR_EOF;
}
