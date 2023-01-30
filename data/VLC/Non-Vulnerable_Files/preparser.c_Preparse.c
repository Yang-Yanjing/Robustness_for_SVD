 */
static void Preparse( vlc_object_t *obj, input_item_t *p_item )
{
    vlc_mutex_lock( &p_item->lock );
    int i_type = p_item->i_type;
    vlc_mutex_unlock( &p_item->lock );
    if( i_type != ITEM_TYPE_FILE )
    {
        input_item_SetPreparsed( p_item, true );
        return;
    }
    /* Do not preparse if it is already done (like by playing it) */
    if( !input_item_IsPreparsed( p_item ) )
    {
        input_Preparse( obj, p_item );
        input_item_SetPreparsed( p_item, true );
        var_SetAddress( obj, "item-change", p_item );
    }
}
