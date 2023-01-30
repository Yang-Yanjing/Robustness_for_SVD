 *****************************************************************************/
int csa_UseKey( vlc_object_t *p_caller, csa_t *c, bool use_odd )
{
    if ( !c ) return VLC_ENOOBJ;
    c->use_odd = use_odd;
#ifndef TS_NO_CSA_CK_MSG
        msg_Dbg( p_caller, "using the %s key for scrambling",
                 use_odd ? "odd" : "even" );
#endif
    return VLC_SUCCESS;
}
