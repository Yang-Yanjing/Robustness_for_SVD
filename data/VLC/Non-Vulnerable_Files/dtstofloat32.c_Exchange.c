 *****************************************************************************/
static void Exchange( float * p_out, const float * p_in )
{
    int i;
    const float * p_first = p_in + 256;
    const float * p_second = p_in;
    for ( i = 0; i < 256; i++ )
    {
        *p_out++ = *p_first++;
        *p_out++ = *p_second++;
    }
}
