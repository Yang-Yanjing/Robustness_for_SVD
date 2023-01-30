}
static int OutputTimeGet( audio_output_t *aout, mtime_t *delay )
{
    return (TimeGet( &aout->sys->s, delay ) == DS_OK) ? 0 : -1;
}
