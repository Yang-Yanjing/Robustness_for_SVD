}
static void DirectXUnlockSurface(LPDIRECTDRAWSURFACE2 front_surface,
                                 LPDIRECTDRAWSURFACE2 surface)
{
    VLC_UNUSED(front_surface);
    IDirectDrawSurface2_Unlock(surface, NULL);
}
