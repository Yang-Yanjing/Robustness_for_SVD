 */
static void EsOutSelect( es_out_t *out, es_out_id_t *es, bool b_force )
{
    es_out_sys_t      *p_sys = out->p_sys;
    int i_cat = es->fmt.i_cat;
    if( !p_sys->b_active ||
        ( !b_force && es->fmt.i_priority < ES_PRIORITY_SELECTABLE_MIN ) )
    {
        return;
    }
    if( p_sys->i_mode == ES_OUT_MODE_ALL || b_force )
    {
        if( !EsIsSelected( es ) )
            EsSelect( out, es );
    }
    else if( p_sys->i_mode == ES_OUT_MODE_PARTIAL )
    {
        char *prgms = var_GetNonEmptyString( p_sys->p_input, "programs" );
        if( prgms != NULL )
        {
            char *buf;
            for ( const char *prgm = strtok_r( prgms, ",", &buf );
                  prgm != NULL;
                  prgm = strtok_r( NULL, ",", &buf ) )
            {
                if( atoi( prgm ) == es->p_pgrm->i_id || b_force )
                {
                    if( !EsIsSelected( es ) )
                        EsSelect( out, es );
                    break;
                }
            }
            free( prgms );
        }
    }
    else if( p_sys->i_mode == ES_OUT_MODE_AUTO )
    {
        int i_wanted  = -1;
        if( es->p_pgrm != p_sys->p_pgrm )
            return;
        if( i_cat == AUDIO_ES )
        {
            if( p_sys->ppsz_audio_language )
            {
                int es_idx = LanguageArrayIndex( p_sys->ppsz_audio_language,
                                                 es->psz_language_code );
                if( !p_sys->p_es_audio )
                {
                    /* Only select the language if it's in the list */
                    if( es_idx >= 0 )
                        i_wanted = es->i_channel;
                }
                else
                {
                    int selected_es_idx =
                        LanguageArrayIndex( p_sys->ppsz_audio_language,
                                            p_sys->p_es_audio->psz_language_code );
                    if( es_idx >= 0 &&
                        ( selected_es_idx < 0 || es_idx < selected_es_idx ||
                          ( es_idx == selected_es_idx &&
                            p_sys->p_es_audio->fmt.i_priority < es->fmt.i_priority ) ) )
                        i_wanted = es->i_channel;
                }
            }
            else
            {
                /* Select the first one if there is no selected audio yet 
                 * then choose by ES priority */
                if( !p_sys->p_es_audio ||
                    p_sys->p_es_audio->fmt.i_priority < es->fmt.i_priority )
                    i_wanted = es->i_channel;
            }
            if( p_sys->i_audio_last >= 0 )
                i_wanted = p_sys->i_audio_last;
            if( p_sys->i_audio_id >= 0 )
            {
                if( es->i_id == p_sys->i_audio_id )
                    i_wanted = es->i_channel;
                else
                    return;
            }
        }
        else if( i_cat == SPU_ES )
        {
            if( p_sys->ppsz_sub_language )
            {
                int es_idx = LanguageArrayIndex( p_sys->ppsz_sub_language,
                                     es->psz_language_code );
                if( !p_sys->p_es_sub )
                {
                    /* Select the language if it's in the list */
                    if( es_idx >= 0 ||
                        /*FIXME: Should default subtitle not in the list be 
                         * displayed if not forbidden by none? */
                        ( p_sys->i_default_sub_id >= 0 &&
                          /* check if the subtitle isn't forbidden by none */
                          LanguageArrayIndex( p_sys->ppsz_sub_language, "none" ) < 0 &&
                          es->i_id == p_sys->i_default_sub_id ) )
                        i_wanted = es->i_channel;
                }
                else
                {
                    int selected_es_idx =
                        LanguageArrayIndex( p_sys->ppsz_sub_language,
                                            p_sys->p_es_sub->psz_language_code );
                    if( es_idx >= 0 &&
                        ( selected_es_idx < 0 || es_idx < selected_es_idx ||
                          ( es_idx == selected_es_idx &&
                            p_sys->p_es_sub->fmt.i_priority < es->fmt.i_priority ) ) )
                        i_wanted = es->i_channel;
                }
            }
            else
            {
                /* If there is no user preference, select the default subtitle 
                 * or adapt by ES priority */
                if( ( !p_sys->p_es_sub &&
                      ( p_sys->i_default_sub_id >= 0 &&
                        es->i_id == p_sys->i_default_sub_id ) ) ||
                    ( p_sys->p_es_sub && 
                      p_sys->p_es_sub->fmt.i_priority < es->fmt.i_priority ) )
                    i_wanted = es->i_channel;
                else if( p_sys->p_es_sub &&
                         p_sys->p_es_sub->fmt.i_priority >= es->fmt.i_priority )
                    i_wanted = p_sys->p_es_sub->i_channel;
            }
            if( p_sys->i_sub_last >= 0 )
                i_wanted  = p_sys->i_sub_last;
            if( p_sys->i_sub_id >= 0 )
            {
                if( es->i_id == p_sys->i_sub_id )
                    i_wanted = es->i_channel;
                else
                    return;
            }
        }
        else if( i_cat == VIDEO_ES )
        {
            i_wanted  = es->i_channel;
        }
        if( i_wanted == es->i_channel && !EsIsSelected( es ) )
            EsSelect( out, es );
    }
    /* FIXME TODO handle priority here */
    if( EsIsSelected( es ) )
    {
        if( i_cat == AUDIO_ES )
        {
            if( p_sys->i_mode == ES_OUT_MODE_AUTO &&
                p_sys->p_es_audio &&
                p_sys->p_es_audio != es &&
                EsIsSelected( p_sys->p_es_audio ) )
            {
                EsUnselect( out, p_sys->p_es_audio, false );
            }
            p_sys->p_es_audio = es;
        }
        else if( i_cat == SPU_ES )
        {
            if( p_sys->i_mode == ES_OUT_MODE_AUTO &&
                p_sys->p_es_sub &&
                p_sys->p_es_sub != es &&
                EsIsSelected( p_sys->p_es_sub ) )
            {
                EsUnselect( out, p_sys->p_es_sub, false );
            }
            p_sys->p_es_sub = es;
        }
        else if( i_cat == VIDEO_ES )
        {
            p_sys->p_es_video = es;
        }
    }
}
