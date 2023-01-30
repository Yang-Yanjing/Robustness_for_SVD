 *****************************************************************************/
static void Duplicate( float * p_out, const float * p_in )
{
    int i;
    for ( i = 256; i--; )
    {
        *p_out++ = *p_in;
        *p_out++ = *p_in;
        p_in++;
    }
}
