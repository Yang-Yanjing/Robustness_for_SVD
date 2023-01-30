 *****************************************************************************/
static int FindCamera( demux_sys_t *sys, demux_t *p_demux )
{
    dc1394camera_list_t *list;
    int i_ret = VLC_EGENERIC;
    msg_Dbg( p_demux, "Scanning for ieee1394 ports ..." );
    if( dc1394_camera_enumerate (sys->p_dccontext, &list) != DC1394_SUCCESS )
    {
        msg_Err(p_demux, "Can not ennumerate cameras");
        goto end;
    }
    if( list->num == 0 )
    {
        msg_Err(p_demux, "Can not find cameras");
        goto end;
    }
    sys->num_cameras = list->num;
    msg_Dbg( p_demux, "Found %d dc1394 cameras.", list->num);
    if( sys->selected_uid )
    {
        int found = 0;
        for( unsigned i = 0; i < sys->num_cameras; i++ )
        {
            if( list->ids[i].guid == sys->selected_uid )
            {
                sys->camera = dc1394_camera_new(sys->p_dccontext,
                                                list->ids[i].guid);
                found++;
                break;
            }
        }
        if( !found )
        {
            msg_Err( p_demux, "Can't find camera with uid : 0x%"PRIx64".",
                     sys->selected_uid );
            goto end;
        }
    }
    else if( sys->selected_camera >= (int)list->num )
    {
        msg_Err( p_demux, "There are not this many cameras. (%d/%d)",
                 sys->selected_camera, sys->num_cameras );
        goto end;
    }
    else if( sys->selected_camera >= 0 )
    {
        sys->camera = dc1394_camera_new(sys->p_dccontext,
                    list->ids[sys->selected_camera].guid);
    }
    else
    {
        sys->camera = dc1394_camera_new(sys->p_dccontext,
                                          list->ids[0].guid);
    }
    i_ret = VLC_SUCCESS;
end:
    dc1394_camera_free_list (list);
    return i_ret;
}
