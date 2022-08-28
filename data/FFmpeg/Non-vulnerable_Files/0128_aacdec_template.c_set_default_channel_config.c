static int set_default_channel_config(AVCodecContext *avctx,
                                      uint8_t (*layout_map)[3],
                                      int *tags,
                                      int channel_config)
{
    if (channel_config < 1 || (channel_config > 7 && channel_config < 11) ||
        channel_config > 12) {
        av_log(avctx, AV_LOG_ERROR,
               "invalid default channel configuration (%d)\n",
               channel_config);
        return AVERROR_INVALIDDATA;
    }
    *tags = tags_per_config[channel_config];
    memcpy(layout_map, aac_channel_layout_map[channel_config - 1],
           *tags * sizeof(*layout_map));
    










    if (channel_config == 7 && avctx->strict_std_compliance < FF_COMPLIANCE_STRICT) {
        av_log(avctx, AV_LOG_INFO, "Assuming an incorrectly encoded 7.1 channel layout"
               " instead of a spec-compliant 7.1(wide) layout, use -strict %d to decode"
               " according to the specification instead.\n", FF_COMPLIANCE_STRICT);
        layout_map[2][2] = AAC_CHANNEL_SIDE;
    }
    return 0;
}
 */
static int set_default_channel_config(AVCodecContext *avctx,
                                      uint8_t (*layout_map)[3],
                                      int *tags,
                                      int channel_config)
{
    if (channel_config < 1 || (channel_config > 7 && channel_config < 11) ||
        channel_config > 12) {
        av_log(avctx, AV_LOG_ERROR,
               "invalid default channel configuration (%d)\n",
               channel_config);
        return AVERROR_INVALIDDATA;
    }
