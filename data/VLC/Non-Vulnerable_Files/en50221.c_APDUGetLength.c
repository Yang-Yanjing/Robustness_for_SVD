 *****************************************************************************/
static uint8_t *APDUGetLength( uint8_t *p_apdu, int *pi_size )
{
    return GetLength( &p_apdu[3], pi_size );
}
