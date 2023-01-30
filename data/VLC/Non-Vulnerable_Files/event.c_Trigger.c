 *****************************************************************************/
static void Trigger( input_thread_t *p_input, int i_type )
{
    var_SetInteger( p_input, "intf-event", i_type );
}
