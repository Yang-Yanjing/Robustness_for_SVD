}
static int DirectXLock(picture_t *picture)
{
    DDSURFACEDESC ddsd;
    if (DirectXLockSurface(picture->p_sys->front_surface,
                           picture->p_sys->surface, &ddsd))
        return CommonUpdatePicture(picture, &picture->p_sys->fallback, NULL, 0);
    CommonUpdatePicture(picture, NULL, ddsd.lpSurface, ddsd.lPitch);
    return VLC_SUCCESS;
}
