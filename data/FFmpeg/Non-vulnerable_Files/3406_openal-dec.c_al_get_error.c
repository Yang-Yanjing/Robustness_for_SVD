static inline int al_get_error(ALCdevice *device, const char** error_msg_ret)
{
    ALCenum error = alcGetError(device);
    if (error_msg_ret)
        *error_msg_ret = (const char*) alcGetString(device, error);
    switch (error) {
    case ALC_NO_ERROR:
        return 0;
    case ALC_INVALID_DEVICE:
        return AVERROR(ENODEV);
        break;
    case ALC_INVALID_CONTEXT:
    case ALC_INVALID_ENUM:
    case ALC_INVALID_VALUE:
        return AVERROR(EINVAL);
        break;
    case ALC_OUT_OF_MEMORY:
        return AVERROR(ENOMEM);
        break;
    default:
        return AVERROR(EIO);
    }
}
