}
static uint32_t decode_BCD( uint32_t input )
{
    uint32_t output = 0;
    for( short index=28; index >= 0 ; index -= 4 )
    {
        output *= 10;
        output += ((input >> index) & 0x0f);
    };
    return output;
}
