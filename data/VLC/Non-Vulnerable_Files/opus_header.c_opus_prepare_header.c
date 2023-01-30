}
int opus_prepare_header(unsigned channels, unsigned rate, OpusHeader *header)
{
    header->version = 1;
    header->channels = channels;
    header->nb_streams = header->channels;
    header->nb_coupled = 0;
    header->input_sample_rate = rate;
    header->gain = 0; // 0dB
    header->channel_mapping = header->channels > 8 ? 255 :
                              header->channels > 2;
    return 0;
}
