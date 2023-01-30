}
static int RenderText( filter_t *p_filter, subpicture_region_t *p_region_out,
                       subpicture_region_t *p_region_in,
                       const vlc_fourcc_t *p_chroma_list )
{
    return RenderCommon( p_filter, p_region_out, p_region_in, false, p_chroma_list );
}
