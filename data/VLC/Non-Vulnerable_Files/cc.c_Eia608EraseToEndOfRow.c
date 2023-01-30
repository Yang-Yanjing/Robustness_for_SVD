}
static void Eia608EraseToEndOfRow( eia608_t *h )
{
    if( h->mode == EIA608_MODE_TEXT )
        return;
    Eia608ClearScreenRowX( h, Eia608GetWritingScreenIndex( h ), h->cursor.i_row, h->cursor.i_column );
}
