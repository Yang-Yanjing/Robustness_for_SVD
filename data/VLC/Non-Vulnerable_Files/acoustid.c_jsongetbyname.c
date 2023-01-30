}
static json_value * jsongetbyname( json_value *object, const char *psz_name )
{
    if ( object->type != json_object ) return NULL;
    for ( unsigned int i=0; i < object->u.object.length; i++ )
        if ( strcmp( object->u.object.values[i].name, psz_name ) == 0 )
            return object->u.object.values[i].value;
    return NULL;
}
