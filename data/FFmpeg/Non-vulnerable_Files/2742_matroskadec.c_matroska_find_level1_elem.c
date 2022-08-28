static MatroskaLevel1Element *matroska_find_level1_elem(MatroskaDemuxContext *matroska,
                                                        uint32_t id)
{
    int i;
    MatroskaLevel1Element *elem;
    if (!is_ebml_id_valid(id))
        return NULL;
    
    if (id == MATROSKA_ID_CLUSTER)
        return NULL;
    
    if (id != MATROSKA_ID_SEEKHEAD) {
        for (i = 0; i < matroska->num_level1_elems; i++) {
            if (matroska->level1_elems[i].id == id)
                return &matroska->level1_elems[i];
        }
    }
    
    
    
    if (matroska->num_level1_elems >= FF_ARRAY_ELEMS(matroska->level1_elems)) {
        av_log(matroska->ctx, AV_LOG_ERROR, "Too many level1 elements or circular seekheads.\n");
        return NULL;
    }
    elem = &matroska->level1_elems[matroska->num_level1_elems++];
    *elem = (MatroskaLevel1Element){.id = id};
    return elem;
}
