}
static int DirectXCheckLockingSurface(LPDIRECTDRAWSURFACE2 front_surface,
                                      LPDIRECTDRAWSURFACE2 surface)
{
    if (DirectXLockSurface(front_surface, surface, NULL))
        return VLC_EGENERIC;
    DirectXUnlockSurface(front_surface, surface);
    return VLC_SUCCESS;
}
