 *****************************************************************************/
static void InputUpdateMeta( input_thread_t *p_input, demux_t *p_demux )
{
    vlc_meta_t *p_meta = vlc_meta_New();
    if( unlikely(p_meta == NULL) )
        return;
    demux_Control( p_demux, DEMUX_GET_META, p_meta );
    /* If metadata changed, then the attachments might have changed.
       We need to update them in case they contain album art. */
    input_attachment_t **attachment;
    int i_attachment;
    if( !demux_Control( p_demux, DEMUX_GET_ATTACHMENTS,
                        &attachment, &i_attachment ) )
    {
        vlc_mutex_lock( &p_input->p->p_item->lock );
        if( p_input->p->i_attachment > 0 )
        {
            int j = 0;
            for( int i = 0; i < p_input->p->i_attachment; i++ )
            {
                if( p_input->p->attachment_demux[i] == p_demux )
                    vlc_input_attachment_Delete( p_input->p->attachment[i] );
                else
                {
                    p_input->p->attachment[j] = p_input->p->attachment[i];
                    p_input->p->attachment_demux[j] = p_input->p->attachment_demux[i];
                    j++;
                }
            }
            p_input->p->i_attachment = j;
        }
        AppendAttachment( &p_input->p->i_attachment, &p_input->p->attachment, &p_input->p->attachment_demux,
                          i_attachment, attachment, p_demux );
        vlc_mutex_unlock( &p_input->p->p_item->lock );
    }
    es_out_ControlSetMeta( p_input->p->p_es_out, p_meta );
    vlc_meta_Delete( p_meta );
}
