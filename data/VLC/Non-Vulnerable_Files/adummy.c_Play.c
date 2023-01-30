#define A52_FRAME_NB 1536
static void Play(audio_output_t *aout, block_t *block)
{
    block_Release( block );
    (void) aout;
}
