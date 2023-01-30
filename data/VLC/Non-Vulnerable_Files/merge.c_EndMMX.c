#if defined(CAN_COMPILE_MMXEXT) || defined(CAN_COMPILE_SSE)
void EndMMX( void )
{
    __asm__ __volatile__( "emms" :: );
}
