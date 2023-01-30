static int matroska_parse_cluster(MatroskaDemuxContext *matroska)
{
    MatroskaCluster cluster = { 0 };
    EbmlList *blocks_list;
    MatroskaBlock *blocks;
    int i, res;
    int64_t pos;
    if (!matroska->contains_ssa)
        return matroska_parse_cluster_incremental(matroska);
    pos = avio_tell(matroska->ctx->pb);
    matroska->prev_pkt = NULL;
    if (matroska->current_id)
        pos -= 4;  
    res         = ebml_parse(matroska, matroska_clusters, &cluster);
    blocks_list = &cluster.blocks;
    blocks      = blocks_list->elem;
    for (i = 0; i < blocks_list->nb_elem; i++)
        if (blocks[i].bin.size > 0 && blocks[i].bin.data) {
            int is_keyframe = blocks[i].non_simple ? !blocks[i].reference : -1;
            res = matroska_parse_block(matroska, blocks[i].bin.data,
                                       blocks[i].bin.size, blocks[i].bin.pos,
                                       cluster.timecode, blocks[i].duration,
                                       is_keyframe, NULL, 0, 0, pos,
                                       blocks[i].discard_padding);
        }
    ebml_free(matroska_cluster, &cluster);
    return res;
}
