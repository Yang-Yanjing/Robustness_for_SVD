}
static inline float clip_unit( float f )
{
    return f < 0.0 ? 0.0 : ( f > 1.0 ? 1.0 : f );
}
