 *****************************************************************************/
static void CmdClean( ts_cmd_t *p_cmd )
{
    switch( p_cmd->i_type )
    {
    case C_ADD:
        CmdCleanAdd( p_cmd );
        break;
    case C_SEND:
        CmdCleanSend( p_cmd );
        break;
    case C_CONTROL:
        CmdCleanControl( p_cmd );
        break;
    case C_DEL:
        break;
    default:
        assert(0);
        break;
    }
}
