 */
void input_Close( input_thread_t *p_input )
{
    input_Join( p_input );
    input_Release( p_input );
}
