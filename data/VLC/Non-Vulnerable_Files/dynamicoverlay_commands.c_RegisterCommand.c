};
void RegisterCommand( filter_t *p_filter )
{
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    size_t i_index = 0;
    p_sys->i_commands = ARRAY_SIZE(p_commands);
    p_sys->pp_commands = (commanddesc_t **) calloc( p_sys->i_commands, sizeof(commanddesc_t*) );
    if( !p_sys->pp_commands ) return;
    for( i_index = 0; i_index < p_sys->i_commands; i_index ++ )
    {
        p_sys->pp_commands[i_index] = (commanddesc_t *) malloc( sizeof(commanddesc_t) );
        if( !p_sys->pp_commands[i_index] ) return;
        p_sys->pp_commands[i_index]->psz_command = strdup( p_commands[i_index].psz_command );
        p_sys->pp_commands[i_index]->b_atomic = p_commands[i_index].b_atomic;
        p_sys->pp_commands[i_index]->pf_parser = p_commands[i_index].pf_parser;
        p_sys->pp_commands[i_index]->pf_execute = p_commands[i_index].pf_execute;
        p_sys->pp_commands[i_index]->pf_unparse = p_commands[i_index].pf_unparse;
    }
    msg_Dbg( p_filter, "%zu commands are available", p_sys->i_commands );
    for( size_t i_index = 0; i_index < p_sys->i_commands; i_index++ )
        msg_Dbg( p_filter, "    %s", p_sys->pp_commands[i_index]->psz_command );
}
