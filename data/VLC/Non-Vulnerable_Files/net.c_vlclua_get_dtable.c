#include "misc.h"
static vlclua_dtable_t *vlclua_get_dtable( lua_State *L )
{
    return vlclua_get_object( L, vlclua_get_dtable );
}
