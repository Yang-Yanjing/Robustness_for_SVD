 *****************************************************************************/
static void EsOutDel( es_out_t *out, es_out_id_t *es )
{
    es_out_sys_t *p_sys = out->p_sys;
    bool b_reselect = false;
    int i;
    vlc_mutex_lock( &p_sys->lock );
    /* We don't try to reselect */
    if( es->p_dec )
    {
        while( vlc_object_alive(p_sys->p_input) && !p_sys->b_buffering && es->p_dec )
        {
            if( input_DecoderIsEmpty( es->p_dec ) &&
                ( !es->p_dec_record || input_DecoderIsEmpty( es->p_dec_record ) ))
                break;
            /* FIXME there should be a way to have auto deleted es, but there will be
             * a problem when another codec of the same type is created (mainly video) */
            msleep( 20*1000 );
        }
        EsUnselect( out, es, es->p_pgrm == p_sys->p_pgrm );
    }
    if( es->p_pgrm == p_sys->p_pgrm )
        EsOutESVarUpdate( out, es, true );
    TAB_REMOVE( p_sys->i_es, p_sys->es, es );
    /* Update program */
    es->p_pgrm->i_es--;
    if( es->p_pgrm->i_es == 0 )
        msg_Dbg( p_sys->p_input, "Program doesn't contain anymore ES" );
    if( es->b_scrambled )
        EsOutProgramUpdateScrambled( out, es->p_pgrm );
    /* */
    if( p_sys->p_es_audio == es || p_sys->p_es_video == es ||
        p_sys->p_es_sub == es ) b_reselect = true;
    if( p_sys->p_es_audio == es ) p_sys->p_es_audio = NULL;
    if( p_sys->p_es_video == es ) p_sys->p_es_video = NULL;
    if( p_sys->p_es_sub   == es ) p_sys->p_es_sub   = NULL;
    switch( es->fmt.i_cat )
    {
        case AUDIO_ES:
            p_sys->i_audio--;
            break;
        case SPU_ES:
            p_sys->i_sub--;
            break;
        case VIDEO_ES:
            p_sys->i_video--;
            break;
    }
    /* Re-select another track when needed */
    if( b_reselect )
    {
        for( i = 0; i < p_sys->i_es; i++ )
        {
            if( es->fmt.i_cat == p_sys->es[i]->fmt.i_cat )
                EsOutSelect( out, p_sys->es[i], false );
        }
    }
    free( es->psz_language );
    free( es->psz_language_code );
    es_format_Clean( &es->fmt );
    vlc_mutex_unlock( &p_sys->lock );
    free( es );
}
