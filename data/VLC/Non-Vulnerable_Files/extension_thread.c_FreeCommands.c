/** Recursively drop and free commands starting from "command" */
static void FreeCommands( struct command_t *command )
{
    if( !command ) return;
    struct command_t *next = command->next;
    switch( command->i_command )
    {
        case CMD_ACTIVATE:
        case CMD_DEACTIVATE:
        case CMD_CLICK: // Arg1 must not be freed
            break;
        case CMD_TRIGGERMENU:
        case CMD_PLAYING_CHANGED:
            free( command->data[0] ); // Arg1 is int*, to free
            break;
        default:
            break;
    }
    free( command );
    FreeCommands( next );
}
