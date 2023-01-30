}
static void DirectXDestroySurface(LPDIRECTDRAWSURFACE2 surface)
{
    IDirectDrawSurface2_Release(surface);
}
