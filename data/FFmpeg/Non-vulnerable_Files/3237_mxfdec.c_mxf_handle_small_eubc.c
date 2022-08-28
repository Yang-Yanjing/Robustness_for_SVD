static void mxf_handle_small_eubc(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    

    if (mxf->op != OPAtom)
        return;
    
    if (s->nb_streams != 1                                     ||
        s->streams[0]->codec->codec_type != AVMEDIA_TYPE_AUDIO ||
        !is_pcm(s->streams[0]->codec->codec_id)                ||
        mxf->nb_index_tables != 1                              ||
        mxf->index_tables[0].nb_segments != 1                  ||
        mxf->index_tables[0].segments[0]->edit_unit_byte_count >= 32)
        return;
    
    


    mxf->edit_units_per_packet = 1920;
}
