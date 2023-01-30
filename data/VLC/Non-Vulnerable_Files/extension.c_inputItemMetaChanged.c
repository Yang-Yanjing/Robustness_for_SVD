 **/
static void inputItemMetaChanged( const vlc_event_t *p_event,
                                  void *data )
{
    assert( p_event && p_event->type == vlc_InputItemMetaChanged );
    extension_t *p_ext = ( extension_t* ) data;
    assert( p_ext != NULL );
    PushCommandUnique( p_ext, CMD_UPDATE_META );
}
