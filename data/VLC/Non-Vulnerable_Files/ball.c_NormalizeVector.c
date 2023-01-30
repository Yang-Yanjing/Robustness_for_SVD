 *****************************************************************************/
static void NormalizeVector( float *vect_x, float *vect_y )
{
    float norm = sqrt( *vect_x * *vect_x + *vect_y * *vect_y );
    if( *vect_x != 0 || *vect_y != 0 )
    {
        *vect_x /= norm;
        *vect_y /= norm;
    }
}
