 */
input_item_t *input_GetItem( input_thread_t *p_input )
{
    assert( p_input && p_input->p );
    return p_input->p->p_item;
}
