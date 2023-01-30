}
static bool Eia608ParseCommand0x14( eia608_t *h, uint8_t d2 )
{
    bool b_changed = false;
    switch( d2 )
    {
    case 0x20:  /* Resume caption loading */
        h->mode = EIA608_MODE_POPUP;
        break;
    case 0x21:  /* Backspace */
        Eia608Erase( h );
        b_changed = true;
        break;
    case 0x22:  /* Reserved */
    case 0x23:
        break;
    case 0x24:  /* Delete to end of row */
        Eia608EraseToEndOfRow( h );
        break;
    case 0x25:  /* Rollup 2 */
    case 0x26:  /* Rollup 3 */
    case 0x27:  /* Rollup 4 */
        if( h->mode == EIA608_MODE_POPUP || h->mode == EIA608_MODE_PAINTON )
        {
            Eia608EraseScreen( h, true );
            Eia608EraseScreen( h, false );
            b_changed = true;
        }
        if( d2 == 0x25 )
            h->mode = EIA608_MODE_ROLLUP_2;
        else if( d2 == 0x26 )
            h->mode = EIA608_MODE_ROLLUP_3;
        else
            h->mode = EIA608_MODE_ROLLUP_4;
        h->cursor.i_column = 0;
        h->cursor.i_row = h->i_row_rollup;
        break;
    case 0x28:  /* Flash on */
        /* TODO */
        break;
    case 0x29:  /* Resume direct captionning */
        h->mode = EIA608_MODE_PAINTON;
        break;
    case 0x2a:  /* Text restart */
        /* TODO */
        break;
    case 0x2b: /* Resume text display */
        h->mode = EIA608_MODE_TEXT;
        break;
    case 0x2c: /* Erase displayed memory */
        Eia608EraseScreen( h, true );
        b_changed = true;
        break;
    case 0x2d: /* Carriage return */
        Eia608RollUp(h);
        b_changed = true;
        break;
    case 0x2e: /* Erase non displayed memory */
        Eia608EraseScreen( h, false );
        break;
    case 0x2f: /* End of caption (flip screen if not paint on) */
        if( h->mode != EIA608_MODE_PAINTON )
            h->i_screen = 1 - h->i_screen;
        h->mode = EIA608_MODE_POPUP;
        h->cursor.i_column = 0;
        h->cursor.i_row = 0;
        h->color = EIA608_COLOR_DEFAULT;
        h->font = EIA608_FONT_REGULAR;
        b_changed = true;
        break;
    }
    return b_changed;
}
