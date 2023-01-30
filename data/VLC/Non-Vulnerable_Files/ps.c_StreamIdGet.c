}
static int StreamIdGet( bool *id, int i_id_min, int i_id_max )
{
    int i;
    for( i = 0; i <= i_id_max - i_id_min; i++ )
    {
        if( id[i] )
        {
            id[i] = false;
            return i_id_min + i;
        }
    }
    return -1;
}
