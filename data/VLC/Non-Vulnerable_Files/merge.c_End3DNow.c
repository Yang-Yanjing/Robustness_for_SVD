#if defined(CAN_COMPILE_3DNOW)
void End3DNow( void )
{
    __asm__ __volatile__( "femms" :: );
}
