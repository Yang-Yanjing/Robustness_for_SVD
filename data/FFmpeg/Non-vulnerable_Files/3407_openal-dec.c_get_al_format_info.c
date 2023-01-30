static const inline al_format_info* get_al_format_info(ALCenum al_fmt)
{
    static const al_format_info info_table[] = {
        [AL_FORMAT_MONO8-LOWEST_AL_FORMAT]    = {AL_FORMAT_MONO8, AV_CODEC_ID_PCM_U8, 1},
        [AL_FORMAT_MONO16-LOWEST_AL_FORMAT]   = {AL_FORMAT_MONO16, AV_NE (AV_CODEC_ID_PCM_S16BE, AV_CODEC_ID_PCM_S16LE), 1},
        [AL_FORMAT_STEREO8-LOWEST_AL_FORMAT]  = {AL_FORMAT_STEREO8, AV_CODEC_ID_PCM_U8, 2},
        [AL_FORMAT_STEREO16-LOWEST_AL_FORMAT] = {AL_FORMAT_STEREO16, AV_NE (AV_CODEC_ID_PCM_S16BE, AV_CODEC_ID_PCM_S16LE), 2},
    };
    return &info_table[al_fmt-LOWEST_AL_FORMAT];
}
