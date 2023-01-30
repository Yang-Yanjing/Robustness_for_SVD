}
static void SplitterPictureDel(video_splitter_t *splitter, picture_t *picture[])
{
    vout_display_sys_t *wsys = splitter->p_owner->wrapper->sys;
    for (int i = 0; i < wsys->count; i++)
        picture_Release(picture[i]);
}
