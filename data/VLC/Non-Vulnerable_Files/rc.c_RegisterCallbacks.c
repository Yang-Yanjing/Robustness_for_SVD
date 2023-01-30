 *****************************************************************************/
static void RegisterCallbacks( intf_thread_t *p_intf )
{
    /* Register commands that will be cleaned up upon object destruction */
#define ADD( name, type, target )                                   \
    var_Create( p_intf, name, VLC_VAR_ ## type | VLC_VAR_ISCOMMAND ); \
    var_AddCallback( p_intf, name, target, NULL );
    ADD( "quit", VOID, Quit )
    ADD( "intf", STRING, Intf )
    ADD( "add", STRING, Playlist )
    ADD( "repeat", STRING, Playlist )
    ADD( "loop", STRING, Playlist )
    ADD( "random", STRING, Playlist )
    ADD( "enqueue", STRING, Playlist )
    ADD( "playlist", VOID, Playlist )
    ADD( "sort", VOID, Playlist )
    ADD( "play", VOID, Playlist )
    ADD( "stop", VOID, Playlist )
    ADD( "clear", VOID, Playlist )
    ADD( "prev", VOID, Playlist )
    ADD( "next", VOID, Playlist )
    ADD( "goto", INTEGER, Playlist )
    ADD( "status", INTEGER, Playlist )
    /* DVD commands */
    ADD( "pause", VOID, Input )
    ADD( "seek", INTEGER, Input )
    ADD( "title", STRING, Input )
    ADD( "title_n", VOID, Input )
    ADD( "title_p", VOID, Input )
    ADD( "chapter", STRING, Input )
    ADD( "chapter_n", VOID, Input )
    ADD( "chapter_p", VOID, Input )
    ADD( "fastforward", VOID, Input )
    ADD( "rewind", VOID, Input )
    ADD( "faster", VOID, Input )
    ADD( "slower", VOID, Input )
    ADD( "normal", VOID, Input )
    ADD( "frame", VOID, Input )
    ADD( "atrack", STRING, Input )
    ADD( "vtrack", STRING, Input )
    ADD( "strack", STRING, Input )
    /* video commands */
    ADD( "vratio", STRING, VideoConfig )
    ADD( "vcrop", STRING, VideoConfig )
    ADD( "vzoom", STRING, VideoConfig )
    ADD( "snapshot", VOID, VideoConfig )
    /* audio commands */
    ADD( "volume", STRING, Volume )
    ADD( "volup", STRING, VolumeMove )
    ADD( "voldown", STRING, VolumeMove )
    ADD( "adev", STRING, AudioDevice )
    ADD( "achan", STRING, AudioChannel )
    /* misc menu commands */
    ADD( "stats", BOOL, Statistics )
#undef ADD
}
