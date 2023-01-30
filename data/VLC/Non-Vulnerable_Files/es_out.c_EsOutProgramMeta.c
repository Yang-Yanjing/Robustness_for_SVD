}
static void EsOutProgramMeta( es_out_t *out, int i_group, const vlc_meta_t *p_meta )
{
    es_out_sys_t      *p_sys = out->p_sys;
    es_out_pgrm_t     *p_pgrm;
    input_thread_t    *p_input = p_sys->p_input;
    const char        *psz_title = NULL;
    const char        *psz_provider = NULL;
    int i;
    msg_Dbg( p_input, "EsOutProgramMeta: number=%d", i_group );
    /* Check against empty meta data (empty for what we handle) */
    if( !vlc_meta_Get( p_meta, vlc_meta_Title) &&
        !vlc_meta_Get( p_meta, vlc_meta_NowPlaying) &&
        !vlc_meta_Get( p_meta, vlc_meta_ESNowPlaying) &&
        !vlc_meta_Get( p_meta, vlc_meta_Publisher) &&
        vlc_meta_GetExtraCount( p_meta ) <= 0 )
    {
        return;
    }
    /* Find program */
    if( !EsOutIsProgramVisible( out, i_group ) )
        return;
    p_pgrm = EsOutProgramFind( out, i_group );
    if( !p_pgrm )
        return;
    /* */
    psz_title = vlc_meta_Get( p_meta, vlc_meta_Title);
    psz_provider = vlc_meta_Get( p_meta, vlc_meta_Publisher);
    /* Update the description text of the program */
    if( psz_title && *psz_title )
    {
        if( !p_pgrm->psz_name || strcmp( p_pgrm->psz_name, psz_title ) )
        {
            char *psz_cat = EsOutProgramGetMetaName( p_pgrm );
            /* Remove old entries */
            input_Control( p_input, INPUT_DEL_INFO, psz_cat, NULL );
            /* TODO update epg name ?
             * TODO update scrambled info name ? */
            free( psz_cat );
        }
        free( p_pgrm->psz_name );
        p_pgrm->psz_name = strdup( psz_title );
        char *psz_text;
        if( psz_provider && *psz_provider )
        {
            if( asprintf( &psz_text, "%s [%s]", psz_title, psz_provider ) < 0 )
                psz_text = NULL;
        }
        else
        {
            psz_text = strdup( psz_title );
        }
        /* ugly but it works */
        if( psz_text )
        {
            input_SendEventProgramDel( p_input, i_group );
            input_SendEventProgramAdd( p_input, i_group, psz_text );
            if( p_sys->p_pgrm == p_pgrm )
                input_SendEventProgramSelect( p_input, i_group );
            free( psz_text );
        }
    }
    /* */
    char **ppsz_all_keys = vlc_meta_CopyExtraNames(p_meta );
    info_category_t *p_cat = NULL;
    if( psz_provider || ( ppsz_all_keys[0] && *ppsz_all_keys[0] ) )
    {
        char *psz_cat = EsOutProgramGetMetaName( p_pgrm );
        if( psz_cat )
            p_cat = info_category_New( psz_cat );
        free( psz_cat );
    }
    for( i = 0; ppsz_all_keys[i]; i++ )
    {
        if( p_cat )
            info_category_AddInfo( p_cat, vlc_gettext(ppsz_all_keys[i]), "%s",
                                   vlc_meta_GetExtra( p_meta, ppsz_all_keys[i] ) );
        free( ppsz_all_keys[i] );
    }
    free( ppsz_all_keys );
    if( psz_provider )
    {
        if( p_sys->p_pgrm == p_pgrm )
        {
            input_item_SetPublisher( p_input->p->p_item, psz_provider );
            input_SendEventMeta( p_input );
        }
        if( p_cat )
            info_category_AddInfo( p_cat, vlc_meta_TypeToLocalizedString(vlc_meta_Publisher),
                                   "%s",psz_provider );
    }
    if( p_cat )
        input_Control( p_input, INPUT_MERGE_INFOS, p_cat );
}
