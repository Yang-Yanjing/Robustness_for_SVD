}
static fe_hierarchy_t DecodeHierarchy( access_t *p_access )
{
    fe_hierarchy_t      fe_hierarchy = 0;
    int i_hierarchy = var_GetInteger( p_access, "dvb-hierarchy" );
    msg_Dbg( p_access, "using hierarchy=%d", i_hierarchy );
    switch( i_hierarchy )
    {
        case -1: fe_hierarchy = HIERARCHY_NONE; break;
        case 0: fe_hierarchy = HIERARCHY_AUTO; break;
        case 1: fe_hierarchy = HIERARCHY_1; break;
        case 2: fe_hierarchy = HIERARCHY_2; break;
        case 4: fe_hierarchy = HIERARCHY_4; break;
        default:
            msg_Dbg( p_access, "terrestrial dvb has hierarchy not set, using auto");
            fe_hierarchy = HIERARCHY_AUTO;
            break;
    }
    return fe_hierarchy;
}
