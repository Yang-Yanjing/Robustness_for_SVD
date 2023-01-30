 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_mux_t      *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t  *p_sys;
    msg_Info( p_mux, "Open" );
    p_sys                 = malloc( sizeof( sout_mux_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_streams      = 0;
    p_sys->i_add_streams  = 0;
    p_sys->b_can_add_streams = true;
    p_sys->i_del_streams  = 0;
    p_sys->pp_del_streams = 0;
    p_sys->i_pos = 0;
    p_sys->skeleton.b_create = false;
    p_sys->skeleton.b_head_done = false;
    p_sys->skeleton.i_index_intvl =
            var_InheritInteger( p_this, SOUT_CFG_PREFIX "indexintvl" );
    p_sys->skeleton.i_index_ratio =
            var_InheritFloat( p_this, SOUT_CFG_PREFIX "indexratio" );
    p_sys->i_data_start = 0;
    p_sys->i_segment_start = 0;
    p_mux->p_sys        = p_sys;
    p_mux->pf_control   = Control;
    p_mux->pf_addstream = AddStream;
    p_mux->pf_delstream = DelStream;
    p_mux->pf_mux       = Mux;
    /* First serial number is random.
     * (Done like this because on win32 you need to seed the random number
     *  generator once per thread). */
    uint32_t r;
    vlc_rand_bytes(&r, sizeof(r));
    p_sys->i_next_serial_no = r & INT_MAX;
    return VLC_SUCCESS;
}
