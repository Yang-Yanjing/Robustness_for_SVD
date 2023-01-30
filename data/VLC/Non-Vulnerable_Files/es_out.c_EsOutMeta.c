}
static void EsOutMeta( es_out_t *p_out, const vlc_meta_t *p_meta )
{
    es_out_sys_t    *p_sys = p_out->p_sys;
    input_thread_t  *p_input = p_sys->p_input;
    input_item_t *p_item = input_GetItem( p_input );
    char *psz_title = NULL;
    char *psz_arturl = input_item_GetArtURL( p_item );
    vlc_mutex_lock( &p_item->lock );
    if( vlc_meta_Get( p_meta, vlc_meta_Title ) && !p_item->b_fixed_name )
        psz_title = strdup( vlc_meta_Get( p_meta, vlc_meta_Title ) );
    vlc_meta_Merge( p_item->p_meta, p_meta );
    if( !psz_arturl || *psz_arturl == '\0' )
    {
        const char *psz_tmp = vlc_meta_Get( p_item->p_meta, vlc_meta_ArtworkURL );
        if( psz_tmp )
            psz_arturl = strdup( psz_tmp );
    }
    vlc_mutex_unlock( &p_item->lock );
    if( psz_arturl && *psz_arturl )
    {
        input_item_SetArtURL( p_item, psz_arturl );
        if( !strncmp( psz_arturl, "attachment://", 13 ) )
        {
            /* Don't look for art cover if sout
             * XXX It can change when sout has meta data support */
            if( p_input->p->p_sout && !p_input->b_preparsing )
                input_item_SetArtURL( p_item, "" );
            else
                input_ExtractAttachmentAndCacheArt( p_input );
        }
    }
    free( psz_arturl );
    if( psz_title )
    {
        input_item_SetName( p_item, psz_title );
        free( psz_title );
    }
    input_item_SetPreparsed( p_item, true );
    input_SendEventMeta( p_input );
    /* TODO handle sout meta ? */
}
