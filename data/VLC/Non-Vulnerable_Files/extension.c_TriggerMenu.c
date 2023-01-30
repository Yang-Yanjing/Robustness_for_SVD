}
static inline int TriggerMenu( extension_t *p_ext, int i_id )
{
    return PushCommand( p_ext, CMD_TRIGGERMENU, i_id );
}
