}
static void FeaturesCheck( void *opaque, const char *feature )
{
    ftp_features_t *features = opaque;
    if( strcasestr( feature, "UTF8" ) != NULL )
        features->b_unicode = true;
    else
    if( strcasestr( feature, "AUTH TLS" ) != NULL )
        features->b_authtls = true;
}
