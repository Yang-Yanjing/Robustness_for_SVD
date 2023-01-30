}
char *en50221_Status( cam_t *p_cam, char *psz_request )
{
    if( psz_request != NULL && *psz_request )
    {
        /* Check if we have an undisplayed MMI message : in that case we ignore
         * the user input to avoid confusing the CAM. */
        for ( unsigned i_slot = 0; i_slot < p_cam->i_nb_slots; i_slot++ )
        {
            if ( p_cam->pb_slot_mmi_undisplayed[i_slot] )
            {
                psz_request = NULL;
                msg_Dbg( p_cam->obj,
                         "ignoring user request because of a new MMI object" );
                break;
            }
        }
    }
    if( psz_request != NULL && *psz_request )
    {
        /* We have a mission to accomplish. */
        mmi_t mmi_object;
        char psz_value[255];
        int i_slot;
        bool b_ok = false;
        if ( HTTPExtractValue( psz_request, "slot", psz_value,
                                   sizeof(psz_value) ) == NULL )
        {
            return strdup( "invalid request parameter\n" );
        }
        i_slot = atoi(psz_value);
        if ( HTTPExtractValue( psz_request, "open", psz_value,
                                   sizeof(psz_value) ) != NULL )
        {
            en50221_OpenMMI( p_cam, i_slot );
            return NULL;
        }
        if ( HTTPExtractValue( psz_request, "close", psz_value,
                                   sizeof(psz_value) ) != NULL )
        {
            en50221_CloseMMI( p_cam, i_slot );
            return NULL;
        }
        if ( HTTPExtractValue( psz_request, "cancel", psz_value,
                                   sizeof(psz_value) ) == NULL )
        {
            b_ok = true;
        }
        if ( HTTPExtractValue( psz_request, "type", psz_value,
                                   sizeof(psz_value) ) == NULL )
        {
            return strdup( "invalid request parameter\n" );
        }
        if ( !strcmp( psz_value, "enq" ) )
        {
            mmi_object.i_object_type = EN50221_MMI_ANSW;
            mmi_object.u.answ.b_ok = b_ok;
            if ( !b_ok )
            {
                mmi_object.u.answ.psz_answ = strdup("");
            }
            else
            {
                if ( HTTPExtractValue( psz_request, "answ", psz_value,
                                           sizeof(psz_value) ) == NULL )
                {
                    return strdup( "invalid request parameter\n" );
                }
                mmi_object.u.answ.psz_answ = strdup(psz_value);
            }
        }
        else
        {
            mmi_object.i_object_type = EN50221_MMI_MENU_ANSW;
            if ( !b_ok )
            {
                mmi_object.u.menu_answ.i_choice = 0;
            }
            else
            {
                if ( HTTPExtractValue( psz_request, "choice", psz_value,
                                           sizeof(psz_value) ) == NULL )
                    mmi_object.u.menu_answ.i_choice = 0;
                else
                    mmi_object.u.menu_answ.i_choice = atoi(psz_value);
            }
        }
        en50221_SendMMIObject( p_cam, i_slot, &mmi_object );
        return NULL;
    }
    /* Check that we have all necessary MMI information. */
    for( unsigned i_slot = 0; i_slot < p_cam->i_nb_slots; i_slot++ )
    {
        if ( p_cam->pb_slot_mmi_expected[i_slot] )
            return NULL;
    }
    char *buf;
    size_t len;
    FILE *p = open_memstream( &buf, &len );
    if( unlikely(p == NULL) )
        return NULL;
    ca_caps_t caps;
    if( ioctl( p_cam->fd, CA_GET_CAP, &caps ) < 0 )
    {
        fprintf( p, "ioctl(CA_GET_CAP) failed: %s\n", vlc_strerror_c(errno) );
        goto out;
    }
    /* Output CA capabilities */
    fprintf( p, "CA interface with %d %s, type:\n<table>", caps.slot_num,
             caps.slot_num == 1 ? "slot" : "slots" );
#define CHECK_CAPS( x, s )                                                  \
    if ( caps.slot_type & (CA_##x) )                                        \
        fprintf( p, "<tr><td>%s</td></tr>\n", s )
    CHECK_CAPS( CI, "CI high level interface" );
    CHECK_CAPS( CI_LINK, "CI link layer level interface" );
    CHECK_CAPS( CI_PHYS, "CI physical layer level interface (not supported)" );
    CHECK_CAPS( DESCR, "built-in descrambler" );
    CHECK_CAPS( SC, "simple smartcard interface" );
#undef CHECK_CAPS
    fprintf( p, "</table>%d available %s\n<table>", caps.descr_num,
           caps.descr_num == 1 ? "descrambler (key)" : "descramblers (keys)" );
#define CHECK_DESC( x )                                                     \
    if ( caps.descr_type & (CA_##x) )                                       \
        fprintf( p, "<tr><td>%s</td></tr>", STRINGIFY(x) )
    CHECK_DESC( ECD );
    CHECK_DESC( NDS );
    CHECK_DESC( DSS );
#undef CHECK_DESC
    fputs( "</table>", p );
    for( unsigned i_slot = 0; i_slot < p_cam->i_nb_slots; i_slot++ )
    {
        ca_slot_info_t sinfo;
        p_cam->pb_slot_mmi_undisplayed[i_slot] = false;
        fprintf( p, "<p>CA slot #%d: ", i_slot );
        sinfo.num = i_slot;
        if ( ioctl( p_cam->fd, CA_GET_SLOT_INFO, &sinfo ) < 0 )
        {
            fprintf( p, "ioctl(CA_GET_SLOT_INFO) failed: %s<br>\n",
                     vlc_strerror_c(errno) );
            continue;
        }
#define CHECK_TYPE( x, s )                                                  \
        if ( sinfo.type & (CA_##x) )                                        \
            fputs( s, p )
        CHECK_TYPE( CI, "high level, " );
        CHECK_TYPE( CI_LINK, "link layer level, " );
        CHECK_TYPE( CI_PHYS, "physical layer level, " );
#undef CHECK_TYPE
        if ( sinfo.flags & CA_CI_MODULE_READY )
        {
            mmi_t *p_object = en50221_GetMMIObject( p_cam, i_slot );
            fputs( "module present and ready<p>\n", p );
            fputs( "<form action=index.html method=get>\n", p );
            fprintf( p, "<input type=hidden name=slot value=\"%d\">\n",
                     i_slot );
            if ( p_object == NULL )
            {
                fputs( "<input type=submit name=open"
                       " value=\"Open session\">\n", p );
            }
            else
            {
                switch ( p_object->i_object_type )
                {
                case EN50221_MMI_ENQ:
                    fputs( "<input type=hidden name=type value=enq>\n", p );
                    fprintf( p, "<table border=1><tr><th>%s</th></tr>\n",
                             p_object->u.enq.psz_text );
                    fprintf( p, "<tr><td><input type=%s name=answ>"
                             "</td></tr>\n",
                             p_object->u.enq.b_blind ? "password" : "text" );
                    break;
                case EN50221_MMI_MENU:
                    fputs( "<input type=hidden name=type value=menu>\n", p );
                    fprintf( p, "<table border=1><tr><th>%s</th></tr>\n",
                             p_object->u.menu.psz_title );
                    fprintf( p, "<tr><td>%s</td></tr><tr><td>\n",
                             p_object->u.menu.psz_subtitle );
                    for ( int i = 0; i < p_object->u.menu.i_choices; i++ )
                        fprintf( p, "<input type=radio name=choice"
                                 " value=\"%d\">%s<br>\n", i + 1,
                                 p_object->u.menu.ppsz_choices[i] );
                    fprintf( p, "</td></tr><tr><td>%s</td></tr>\n",
                             p_object->u.menu.psz_bottom );
                    break;
                case EN50221_MMI_LIST:
                    fputs( "<input type=hidden name=type value=menu>\n", p );
                    fputs( "<input type=hidden name=choice value=0>\n", p );
                    fprintf( p, "<table border=1><tr><th>%s</th></tr>\n",
                             p_object->u.menu.psz_title );
                    fprintf( p, "<tr><td>%s</td></tr><tr><td>\n",
                             p_object->u.menu.psz_subtitle );
                    for ( int i = 0; i < p_object->u.menu.i_choices; i++ )
                        fprintf( p, "%s<br>\n",
                                 p_object->u.menu.ppsz_choices[i] );
                    fprintf( p, "</td></tr><tr><td>%s</td></tr>\n",
                             p_object->u.menu.psz_bottom );
                    break;
                default:
                    fputs( "<table><tr><th>Unknown MMI object type</th></tr>\n", p );
                }
                fputs( "</table><p><input type=submit name=ok value=\"OK\">\n", p );
                fputs( "<input type=submit name=cancel value=\"Cancel\">\n", p );
                fputs( "<input type=submit name=close value=\"Close Session\">\n", p );
            }
            fputs( "</form>\n", p );
        }
        else if ( sinfo.flags & CA_CI_MODULE_PRESENT )
            fputs( "module present, not ready<br>\n", p );
        else
            fputs( "module not present<br>\n", p );
    }
out:
    fclose( p );
    return buf;
}
