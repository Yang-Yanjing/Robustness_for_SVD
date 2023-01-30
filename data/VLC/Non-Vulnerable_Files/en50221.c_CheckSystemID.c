} system_ids_t;
static bool CheckSystemID( system_ids_t *p_ids, uint16_t i_id )
{
    int i = 0;
    if( !p_ids ) return true;      /* dummy session for high-level CI intf */
    while ( p_ids->pi_system_ids[i] )
    {
        if ( p_ids->pi_system_ids[i] == i_id )
            return true;
        i++;
    }
    return false;
}
