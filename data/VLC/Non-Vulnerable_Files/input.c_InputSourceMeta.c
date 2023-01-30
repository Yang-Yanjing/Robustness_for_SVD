 *****************************************************************************/
static void InputSourceMeta( input_thread_t *p_input,
                             input_source_t *p_source, vlc_meta_t *p_meta )
{
    demux_t *p_demux = p_source->p_demux;
    /* XXX Remember that checking against p_item->p_meta->i_status & ITEM_PREPARSED
     * is a bad idea */
    bool has_meta = false;
    /* Read demux meta */
    if( !demux_Control( p_demux, DEMUX_GET_META, p_meta ) )
        has_meta = true;
    bool has_unsupported;
    if( demux_Control( p_demux, DEMUX_HAS_UNSUPPORTED_META, &has_unsupported ) )
        has_unsupported = true;
    /* If the demux report unsupported meta data, or if we don't have meta data
     * try an external "meta reader" */
    if( has_meta && !has_unsupported )
        return;
    demux_meta_t *p_demux_meta =
        vlc_custom_create( p_demux, sizeof( *p_demux_meta ), "demux meta" );
    if( !p_demux_meta )
        return;
    p_demux_meta->p_demux = p_demux;
    p_demux_meta->p_item = p_input->p->p_item;
    module_t *p_id3 = module_need( p_demux_meta, "meta reader", NULL, false );
    if( p_id3 )
    {
        if( p_demux_meta->p_meta )
        {
            vlc_meta_Merge( p_meta, p_demux_meta->p_meta );
            vlc_meta_Delete( p_demux_meta->p_meta );
        }
        if( p_demux_meta->i_attachments > 0 )
        {
            vlc_mutex_lock( &p_input->p->p_item->lock );
            AppendAttachment( &p_input->p->i_attachment, &p_input->p->attachment, &p_input->p->attachment_demux,
                              p_demux_meta->i_attachments, p_demux_meta->attachments, p_demux);
            vlc_mutex_unlock( &p_input->p->p_item->lock );
        }
        module_unneed( p_demux, p_id3 );
    }
    vlc_object_release( p_demux_meta );
}
