}
void UnregisterCommand( filter_t *p_filter )
{
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    size_t i_index = 0;
    for( i_index = 0; i_index < p_sys->i_commands; i_index++ )
    {
        free( p_sys->pp_commands[i_index]->psz_command );
        free( p_sys->pp_commands[i_index] );
    }
    free( p_sys->pp_commands );
    p_sys->pp_commands = NULL;
    p_sys->i_commands = 0;
}
