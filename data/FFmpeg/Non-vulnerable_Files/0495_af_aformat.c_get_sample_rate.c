static int get_sample_rate(const char *samplerate)
{
    int ret = strtol(samplerate, NULL, 0);
    return FFMAX(ret, 0);
}
