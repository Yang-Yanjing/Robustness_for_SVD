int ff_audio_data_copy(AudioData *dst, AudioData *src, ChannelMapInfo *map)
{
    int ret, p;
    
    if (dst->sample_fmt != src->sample_fmt || dst->channels < src->channels)
        return AVERROR(EINVAL);
    if (map && !src->is_planar) {
        av_log(src, AV_LOG_ERROR, "cannot remap packed format during copy\n");
        return AVERROR(EINVAL);
    }
    
    if (!src->nb_samples) {
        dst->nb_samples = 0;
        return 0;
    }
    
    ret = ff_audio_data_realloc(dst, src->nb_samples);
    if (ret < 0)
        return ret;
    
    if (map) {
        if (map->do_remap) {
            for (p = 0; p < src->planes; p++) {
                if (map->channel_map[p] >= 0)
                    memcpy(dst->data[p], src->data[map->channel_map[p]],
                           src->nb_samples * src->stride);
            }
        }
        if (map->do_copy || map->do_zero) {
            for (p = 0; p < src->planes; p++) {
                if (map->channel_copy[p])
                    memcpy(dst->data[p], dst->data[map->channel_copy[p]],
                           src->nb_samples * src->stride);
                else if (map->channel_zero[p])
                    av_samples_set_silence(&dst->data[p], 0, src->nb_samples,
                                           1, dst->sample_fmt);
            }
        }
    } else {
        for (p = 0; p < src->planes; p++)
            memcpy(dst->data[p], src->data[p], src->nb_samples * src->stride);
    }
    dst->nb_samples = src->nb_samples;
    return 0;
}
