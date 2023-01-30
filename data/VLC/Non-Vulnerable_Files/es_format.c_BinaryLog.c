 *****************************************************************************/
static int BinaryLog( uint32_t i )
{
    int i_log = 0;
    if( i == 0 ) return -31337;
    if( i & 0xffff0000 ) i_log += 16;
    if( i & 0xff00ff00 ) i_log += 8;
    if( i & 0xf0f0f0f0 ) i_log += 4;
    if( i & 0xcccccccc ) i_log += 2;
    if( i & 0xaaaaaaaa ) i_log += 1;
    return i_log;
}
