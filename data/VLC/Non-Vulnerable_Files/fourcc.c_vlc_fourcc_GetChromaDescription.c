#undef PLANAR
const vlc_chroma_description_t *vlc_fourcc_GetChromaDescription( vlc_fourcc_t i_fourcc )
{
    for( unsigned i = 0; p_list_chroma_description[i].p_fourcc[0]; i++ )
    {
        const vlc_fourcc_t *p_fourcc = p_list_chroma_description[i].p_fourcc;
        for( unsigned j = 0; j < 4 && p_fourcc[j] != 0; j++ )
        {
            if( p_fourcc[j] == i_fourcc )
                return &p_list_chroma_description[i].description;
        }
    }
    return NULL;
}
