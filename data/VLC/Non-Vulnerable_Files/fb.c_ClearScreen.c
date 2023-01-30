};
static void ClearScreen(vout_display_sys_t *sys)
{
    switch (sys->chroma) {
    /* XXX: add other chromas */
    case VLC_CODEC_UYVY: {
        unsigned int j, size = sys->video_size / 4;
        uint32_t *ptr = (uint32_t*)((uintptr_t)(sys->video_ptr + 3) & ~3);
        for(j=0; j < size; j++)
            ptr[j] = 0x10801080;    /* U = V = 16, Y = 128 */
        break;
    }
    default:    /* RGB */
        memset(sys->video_ptr, 0, sys->video_size);
    }
}
