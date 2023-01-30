}
static void DumpVariable (const void *data, const VISIT which, const int depth)
{
    if (which != postorder && which != leaf)
        return;
    (void) depth;
    const variable_t *p_var = *(const variable_t **)data;
    const char *psz_type = "unknown";
    switch( p_var->i_type & VLC_VAR_TYPE )
    {
#define MYCASE( type, nice )    \
        case VLC_VAR_ ## type:  \
            psz_type = nice;    \
            break;
        MYCASE( VOID, "void" );
        MYCASE( BOOL, "bool" );
        MYCASE( INTEGER, "integer" );
        MYCASE( HOTKEY, "hotkey" );
        MYCASE( STRING, "string" );
        MYCASE( VARIABLE, "variable" );
        MYCASE( FLOAT, "float" );
        MYCASE( TIME, "time" );
        MYCASE( COORDS, "coords" );
        MYCASE( ADDRESS, "address" );
#undef MYCASE
    }
    printf( " *-o \"%s\" (%s", p_var->psz_name, psz_type );
    if( p_var->psz_text )
        printf( ", %s", p_var->psz_text );
    fputc( ')', stdout );
    if( p_var->i_type & VLC_VAR_HASCHOICE )
        fputs( ", has choices", stdout );
    if( p_var->i_type & VLC_VAR_ISCOMMAND )
        fputs( ", command", stdout );
    if( p_var->i_entries )
        printf( ", %d callbacks", p_var->i_entries );
    switch( p_var->i_type & VLC_VAR_CLASS )
    {
        case VLC_VAR_VOID:
            break;
        case VLC_VAR_BOOL:
            printf( ": %s", p_var->val.b_bool ? "true" : "false" );
            break;
        case VLC_VAR_INTEGER:
            printf( ": %"PRId64, p_var->val.i_int );
            break;
        case VLC_VAR_STRING:
            printf( ": \"%s\"", p_var->val.psz_string );
            break;
        case VLC_VAR_FLOAT:
            printf( ": %f", p_var->val.f_float );
            break;
        case VLC_VAR_TIME:
            printf( ": %"PRIi64, (int64_t)p_var->val.i_time );
            break;
        case VLC_VAR_COORDS:
            printf( ": %"PRId32"x%"PRId32,
                    p_var->val.coords.x, p_var->val.coords.y );
            break;
        case VLC_VAR_ADDRESS:
            printf( ": %p", p_var->val.p_address );
            break;
    }
    fputc( '\n', stdout );
}
