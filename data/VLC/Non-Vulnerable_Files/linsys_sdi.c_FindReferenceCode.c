#define FIELD_2_ACTIVE_SAV  0xC7
static const uint8_t *FindReferenceCode( uint8_t i_code,
                                         const uint8_t *p_parser,
                                         const uint8_t *p_end )
{
    while ( p_parser <= p_end - 5 )
    {
        if ( p_parser[0] == 0xff && p_parser[1] == 0x3 && p_parser[2] == 0x0
              && p_parser[3] == 0x0 && p_parser[4] == i_code )
            return p_parser;
        p_parser += 5;
    }
    return NULL;
}
