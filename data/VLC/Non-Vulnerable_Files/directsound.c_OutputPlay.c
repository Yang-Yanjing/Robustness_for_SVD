}
static void OutputPlay( audio_output_t *aout, block_t *block )
{
    Play( VLC_OBJECT(aout), &aout->sys->s, block );
}
