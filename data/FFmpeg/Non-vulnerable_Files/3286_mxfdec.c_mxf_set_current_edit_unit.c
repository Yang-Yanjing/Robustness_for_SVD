static int64_t mxf_set_current_edit_unit(MXFContext *mxf, int64_t current_offset)
{
    int64_t last_ofs = -1, next_ofs = -1;
    MXFIndexTable *t = &mxf->index_tables[0];
    

    if (mxf->nb_index_tables <= 0)
        return -1;
    
    while (mxf->current_edit_unit >= 0) {
        if (mxf_edit_unit_absolute_offset(mxf, t, mxf->current_edit_unit + 1, NULL, &next_ofs, 0) < 0)
            return -1;
        if (next_ofs <= last_ofs) {
            

            av_log(mxf->fc, AV_LOG_ERROR,
                   "next_ofs didn't change. not deriving packet timestamps\n");
            return -1;
        }
        if (next_ofs > current_offset)
            break;
        last_ofs = next_ofs;
        mxf->current_edit_unit++;
    }
    
    if (mxf->current_edit_unit < 0)
        return -1;
    return next_ofs;
}
