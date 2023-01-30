}
static void Eia608ClearScreen( eia608_t *h, int i_screen )
{
    for( int i = 0; i < EIA608_SCREEN_ROWS; i++ )
        Eia608ClearScreenRow( h, i_screen, i );
}
