#include "android_opaque.h"
vlc_mutex_t* get_android_opaque_mutex()
{
    static vlc_mutex_t s_mutex = VLC_STATIC_MUTEX;
    return &s_mutex;
}
