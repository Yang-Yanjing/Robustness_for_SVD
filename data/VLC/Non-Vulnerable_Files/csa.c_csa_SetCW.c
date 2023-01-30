 *****************************************************************************/
int csa_SetCW( vlc_object_t *p_caller, csa_t *c, char *psz_ck, bool set_odd )
{
    if ( !c )
    {
        msg_Dbg( p_caller, "no CSA found" );
        return VLC_ENOOBJ;
    }
    /* skip 0x */
    if( psz_ck[0] == '0' && ( psz_ck[1] == 'x' || psz_ck[1] == 'X' ) )
    {
        psz_ck += 2;
    }
    if( strlen( psz_ck ) != 16 )
    {
        msg_Warn( p_caller, "invalid csa ck (it must be 16 chars long)" );
        return VLC_EBADVAR;
    }
    else
    {
        uint64_t i_ck = strtoull( psz_ck, NULL, 16 );
        uint8_t  ck[8];
        int      i;
        for( i = 0; i < 8; i++ )
        {
            ck[i] = ( i_ck >> ( 56 - 8*i) )&0xff;
        }
#ifndef TS_NO_CSA_CK_MSG
        msg_Dbg( p_caller, "using CSA (de)scrambling with %s "
                 "key=%x:%x:%x:%x:%x:%x:%x:%x", set_odd ? "odd" : "even",
                 ck[0], ck[1], ck[2], ck[3], ck[4], ck[5], ck[6], ck[7] );
#endif
        if( set_odd )
        {
            memcpy( c->o_ck, ck, 8 );
            csa_ComputeKey( c->o_kk, ck );
        }
        else
        {
            memcpy( c->e_ck , ck, 8 );
            csa_ComputeKey( c->e_kk , ck );
        }
        return VLC_SUCCESS;
    }
}
