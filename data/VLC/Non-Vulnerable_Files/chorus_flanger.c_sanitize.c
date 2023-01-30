 */
static inline void sanitize( float * f_value )
{
    if ( fabs( *f_value ) < small_value() )
        *f_value = 0.0f;
}
