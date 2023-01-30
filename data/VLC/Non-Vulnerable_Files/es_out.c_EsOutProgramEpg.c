}
static void EsOutProgramEpg( es_out_t *out, int i_group, const vlc_epg_t *p_epg )
{
    es_out_sys_t      *p_sys = out->p_sys;
    input_thread_t    *p_input = p_sys->p_input;
    input_item_t      *p_item = p_input->p->p_item;
    es_out_pgrm_t     *p_pgrm;
    char *psz_cat;
    /* Find program */
    if( !EsOutIsProgramVisible( out, i_group ) )
        return;
    p_pgrm = EsOutProgramFind( out, i_group );
    if( !p_pgrm )
        return;
    /* Update info */
    psz_cat = EsOutProgramGetMetaName( p_pgrm );
    msg_Dbg( p_input, "EsOutProgramEpg: number=%d name=%s", i_group, psz_cat );
    /* Merge EPG */
    vlc_epg_t epg;
    epg = *p_epg;
    epg.psz_name = psz_cat;
    input_item_SetEpg( p_item, &epg );
    input_SendEventMetaEpg( p_sys->p_input );
    /* Update now playing */
    free( p_pgrm->psz_now_playing );
    p_pgrm->psz_now_playing = NULL;
    vlc_mutex_lock( &p_item->lock );
    for( int i = 0; i < p_item->i_epg; i++ )
    {
        const vlc_epg_t *p_tmp = p_item->pp_epg[i];
        if( p_tmp->psz_name && !strcmp(p_tmp->psz_name, psz_cat) )
        {
            if( p_tmp->p_current && p_tmp->p_current->psz_name && *p_tmp->p_current->psz_name )
                p_pgrm->psz_now_playing = strdup( p_tmp->p_current->psz_name );
            break;
        }
    }
    vlc_mutex_unlock( &p_item->lock );
    if( p_pgrm == p_sys->p_pgrm )
    {
        input_item_SetESNowPlaying( p_input->p->p_item, p_pgrm->psz_now_playing );
        input_SendEventMeta( p_input );
    }
    if( p_pgrm->psz_now_playing )
    {
        input_Control( p_input, INPUT_ADD_INFO, psz_cat,
            vlc_meta_TypeToLocalizedString(vlc_meta_ESNowPlaying), "%s",
            p_pgrm->psz_now_playing );
    }
    else
    {
        input_Control( p_input, INPUT_DEL_INFO, psz_cat,
            vlc_meta_TypeToLocalizedString(vlc_meta_ESNowPlaying) );
    }
    free( psz_cat );
}
