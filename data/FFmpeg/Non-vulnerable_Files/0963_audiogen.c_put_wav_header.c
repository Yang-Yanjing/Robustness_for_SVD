static void put_wav_header(int sample_rate, int channels, int nb_samples)
{
    int block_align = SAMPLE_SIZE * channels;
    int data_size   = block_align * nb_samples;
    fputs("RIFF", outfile);
    put32(HEADER_SIZE + data_size);
    fputs("WAVEfmt ", outfile);
    put32(FMT_SIZE);
    put16(WFORMAT_PCM);
    put16(channels);
    put32(sample_rate);
    put32(block_align * sample_rate);
    put16(block_align);
    put16(SAMPLE_SIZE * 8);
    put16(0);
    fputs("data", outfile);
    put32(data_size);
}
