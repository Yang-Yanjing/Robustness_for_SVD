}
static const uint8_t *CountReference( unsigned int *pi_count, uint8_t i_code,
                                      const uint8_t *p_parser,
                                      const uint8_t *p_end )
{
    const uint8_t *p_tmp = FindReferenceCode( i_code, p_parser, p_end );
    if ( p_tmp == NULL )
    {
        *pi_count += p_end - p_parser;
        return NULL;
    }
    *pi_count += p_tmp - p_parser;
    return p_tmp;
}
