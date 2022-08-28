static int matroska_aac_sri(int samplerate)
{
    int sri;
    for (sri = 0; sri < FF_ARRAY_ELEMS(avpriv_mpeg4audio_sample_rates); sri++)
        if (avpriv_mpeg4audio_sample_rates[sri] == samplerate)
            break;
    return sri;
}
