 */
static int Create( vlc_object_t *p_this )
{
    audio_volume_t *p_volume = (audio_volume_t *)p_this;
    switch (p_volume->format)
    {
        case VLC_CODEC_FL32:
            p_volume->amplify = FilterFL32;
            break;
        case VLC_CODEC_FL64:
            p_volume->amplify = FilterFL64;
            break;
        default:
            return -1;
    }
    return 0;
}
