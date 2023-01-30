}
static int Activate (vlc_object_t *obj)
{
    audio_volume_t *vol = (audio_volume_t *)obj;
    switch (vol->format)
    {
        case VLC_CODEC_S32N:
            vol->amplify = FilterS32N;
            break;
        case VLC_CODEC_S16N:
            vol->amplify = FilterS16N;
            break;
        case VLC_CODEC_U8:
            vol->amplify = FilterU8;
            break;
        default:
            return -1;
    }
    return 0;
}
