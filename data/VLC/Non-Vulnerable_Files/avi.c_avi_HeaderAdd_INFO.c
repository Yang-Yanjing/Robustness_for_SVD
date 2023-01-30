}
static int avi_HeaderAdd_INFO( sout_mux_t *p_mux, buffer_out_t *p_bo )
{
    char *psz;
#define APPLY_META(var, fourcc) \
    psz = var_InheritString( p_mux, SOUT_CFG_PREFIX var );\
    if ( psz )\
    {\
        avi_HeaderAdd_meta( p_bo, fourcc, psz );\
        free( psz );\
    }
    AVI_BOX_ENTER_LIST( "INFO" );
    APPLY_META( "artist",   "IART")
    APPLY_META( "comment",  "ICMT")
    APPLY_META( "copyright","ICOP")
    APPLY_META( "date",     "ICRD")
    APPLY_META( "genre",    "IGNR")
    APPLY_META( "name",     "INAM")
    APPLY_META( "keywords", "IKEY")
    APPLY_META( "subject",  "ISBJ")
    APPLY_META( "encoder",  "ISFT")
    /* Some are missing, but are they really useful ?? */
#undef APPLY_META
    AVI_BOX_EXIT( 0 );
}
