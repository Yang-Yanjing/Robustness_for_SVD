}
static VdpStatus Render(VdpDecoder decoder, VdpVideoSurface target,
                        VdpPictureInfo const *picture_info,
                        uint32_t bitstream_buffer_count,
                        VdpBitstreamBuffer const *bitstream_buffers)
{
    (void) decoder; (void) target; (void) picture_info;
    (void) bitstream_buffer_count; (void) bitstream_buffers;
    assert(decoder == DECODER_MAGIC);
    assert(target == DATA_MAGIC);
    return VDP_STATUS_OK;
}
