 *****************************************************************************/
static input_source_t *InputSourceNew( input_thread_t *p_input )
{
    VLC_UNUSED(p_input);
    return calloc( 1,  sizeof( input_source_t ) );
}
