static int matroska_parse_seekhead_entry(MatroskaDemuxContext *matroska,
                                         uint64_t pos)
{
    uint32_t level_up       = matroska->level_up;
    uint32_t saved_id       = matroska->current_id;
    int64_t before_pos = avio_tell(matroska->ctx->pb);
    MatroskaLevel level;
    int64_t offset;
    int ret = 0;
    
    offset = pos + matroska->segment_start;
    if (avio_seek(matroska->ctx->pb, offset, SEEK_SET) == offset) {
        

        if (matroska->num_levels == EBML_MAX_DEPTH) {
            av_log(matroska->ctx, AV_LOG_INFO,
                   "Max EBML element depth (%d) reached, "
                   "cannot parse further.\n", EBML_MAX_DEPTH);
            ret = AVERROR_INVALIDDATA;
        } else {
            level.start  = 0;
            level.length = (uint64_t) -1;
            matroska->levels[matroska->num_levels] = level;
            matroska->num_levels++;
            matroska->current_id                   = 0;
            ret = ebml_parse(matroska, matroska_segment, matroska);
            
            while (matroska->num_levels) {
                uint64_t length = matroska->levels[--matroska->num_levels].length;
                if (length == (uint64_t) -1)
                    break;
            }
        }
    }
    
    avio_seek(matroska->ctx->pb, before_pos, SEEK_SET);
    matroska->level_up   = level_up;
    matroska->current_id = saved_id;
    return ret;
}
