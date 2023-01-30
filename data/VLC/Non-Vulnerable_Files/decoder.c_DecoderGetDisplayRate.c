}
static int DecoderGetDisplayRate( decoder_t *p_dec )
{
    decoder_owner_sys_t *p_owner = p_dec->p_owner;
    if( !p_owner->p_clock )
        return INPUT_RATE_DEFAULT;
    return input_clock_GetRate( p_owner->p_clock );
}
