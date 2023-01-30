#include "misc.h"
void vlclua_set_playlist_internal( lua_State *L, playlist_t *pl )
{
    vlclua_set_object( L, vlclua_set_playlist_internal, pl );
}
