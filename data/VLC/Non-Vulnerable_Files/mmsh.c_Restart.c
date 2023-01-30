/* */
static int Restart( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    char *psz_location = NULL;
    msg_Dbg( p_access, "Restart the stream" );
    p_sys->i_start = p_access->info.i_pos;
    /* */
    msg_Dbg( p_access, "stoping the stream" );
    Stop( p_access );
    /* */
    msg_Dbg( p_access, "describe the stream" );
    if( Describe( p_access, &psz_location ) )
    {
        msg_Err( p_access, "describe failed" );
        return VLC_EGENERIC;
    }
    free( psz_location );
    /* */
    if( Start( p_access, 0 ) )
    {
        msg_Err( p_access, "Start failed" );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
