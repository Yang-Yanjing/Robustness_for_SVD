/* */
static inline int clip_accuracy( int a )
{
    return (a > ACCURACY) ? ACCURACY : (a < 0) ? 0 : a;
}
