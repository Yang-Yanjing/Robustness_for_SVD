}
static picture_t *YCbCrRender(filter_t *filter, picture_t *src)
{
    /* FIXME: Define a way to initialize the mixer in Open() instead. */
    if (unlikely(filter->p_sys->vdp == NULL))
    {
        picture_t *dummy = OutputAllocate(filter);
        if (dummy != NULL)
            picture_Release(dummy);
    }
    src = VideoImport(filter, src);
    return (src != NULL) ? Render(filter, src, true) : NULL;
}
