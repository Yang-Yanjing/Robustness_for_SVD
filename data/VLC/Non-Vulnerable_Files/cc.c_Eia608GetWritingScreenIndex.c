}
static int Eia608GetWritingScreenIndex( eia608_t *h )
{
    switch( h->mode )
    {
    case EIA608_MODE_POPUP:    // Non displayed screen
        return 1 - h->i_screen;
    case EIA608_MODE_ROLLUP_2: // Displayed screen
    case EIA608_MODE_ROLLUP_3:
    case EIA608_MODE_ROLLUP_4:
    case EIA608_MODE_PAINTON:
        return h->i_screen;
    default:
        /* It cannot happen, else it is a bug */
        assert( 0 );
        return 0;
    }
}
