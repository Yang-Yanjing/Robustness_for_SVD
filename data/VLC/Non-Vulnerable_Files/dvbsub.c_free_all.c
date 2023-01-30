}
static void free_all( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    dvbsub_region_t *p_reg, *p_reg_next;
    dvbsub_clut_t *p_clut, *p_clut_next;
    /*free( p_sys->p_display ); No longer malloced */
    for( p_clut = p_sys->p_cluts; p_clut != NULL; p_clut = p_clut_next )
    {
        p_clut_next = p_clut->p_next;
        free( p_clut );
    }
    p_sys->p_cluts = NULL;
    for( p_reg = p_sys->p_regions; p_reg != NULL; p_reg = p_reg_next )
    {
        int i;
        p_reg_next = p_reg->p_next;
        for( i = 0; i < p_reg->i_object_defs; i++ )
            free( p_reg->p_object_defs[i].psz_text );
        if( p_reg->i_object_defs ) free( p_reg->p_object_defs );
        free( p_reg->p_pixbuf );
        free( p_reg );
    }
    p_sys->p_regions = NULL;
    if( p_sys->p_page )
    {
        if( p_sys->p_page->i_region_defs )
            free( p_sys->p_page->p_region_defs );
        free( p_sys->p_page );
    }
    p_sys->p_page = NULL;
}
