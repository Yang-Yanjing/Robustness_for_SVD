#define V 128
static void mmult( double *res, double *a, double *b )
{
    int i, j, k;
    for( i = 0; i < 3; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            res[ i*3 + j ] = 0.;
            for( k = 0; k < 3; k++ )
            {
                res[ i*3 + j ] += a[ i*3 + k ] * b[ k*3 + j ];
            }
        }
    }
}
