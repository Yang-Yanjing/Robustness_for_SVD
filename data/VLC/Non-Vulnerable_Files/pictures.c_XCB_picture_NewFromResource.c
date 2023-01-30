}
picture_t *XCB_picture_NewFromResource (const video_format_t *restrict fmt,
                                        const picture_resource_t *restrict res)
{
    picture_t *pic = picture_NewFromResource (fmt, res);
    if (unlikely(pic == NULL))
        shmdt (res->p[0].p_pixels);
    return pic;
}
