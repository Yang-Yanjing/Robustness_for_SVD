#define AI_ANSWER  0x01
static void MMIFree( mmi_t *p_object )
{
    switch ( p_object->i_object_type )
    {
    case EN50221_MMI_ENQ:
        FREENULL( p_object->u.enq.psz_text );
        break;
    case EN50221_MMI_ANSW:
        if ( p_object->u.answ.b_ok )
        {
            FREENULL( p_object->u.answ.psz_answ );
        }
        break;
    case EN50221_MMI_MENU:
    case EN50221_MMI_LIST:
        FREENULL( p_object->u.menu.psz_title );
        FREENULL( p_object->u.menu.psz_subtitle );
        FREENULL( p_object->u.menu.psz_bottom );
        for ( int i = 0; i < p_object->u.menu.i_choices; i++ )
        {
            free( p_object->u.menu.ppsz_choices[i] );
        }
        FREENULL( p_object->u.menu.ppsz_choices );
        break;
    default:
        break;
    }
}
