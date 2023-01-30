static int mxf_get_essence_container_ul_index(enum AVCodecID id)
{
    int i;
    for (i = 0; mxf_essence_mappings[i].id; i++)
        if (mxf_essence_mappings[i].id == id)
            return mxf_essence_mappings[i].index;
    return -1;
}
