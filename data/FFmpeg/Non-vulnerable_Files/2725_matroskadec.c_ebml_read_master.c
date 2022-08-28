static int ebml_read_master(MatroskaDemuxContext *matroska, uint64_t length)
{
    AVIOContext *pb = matroska->ctx->pb;
    MatroskaLevel *level;
    if (matroska->num_levels >= EBML_MAX_DEPTH) {
        av_log(matroska->ctx, AV_LOG_ERROR,
               "File moves beyond max. allowed depth (%d)\n", EBML_MAX_DEPTH);
        return AVERROR(ENOSYS);
    }
    level         = &matroska->levels[matroska->num_levels++];
    level->start  = avio_tell(pb);
    level->length = length;
    return 0;
}
