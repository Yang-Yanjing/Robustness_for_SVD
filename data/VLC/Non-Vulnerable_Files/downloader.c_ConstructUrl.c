#include "../../demux/mp4/libmp4.h"
static char *ConstructUrl( const char *template, const char *base_url,
        const uint64_t bandwidth, const uint64_t start_time )
{
    char *frag, *end, *qual;
    char *url_template = strdup( template );
    char *saveptr = NULL;
    qual = strtok_r( url_template, "{", &saveptr );
    strtok_r( NULL, "}", &saveptr );
    frag = strtok_r( NULL, "{", &saveptr );
    strtok_r( NULL, "}", &saveptr );
    end = strtok_r( NULL, "", &saveptr );
    char *url = NULL;
    if( asprintf( &url, "%s/%s%"PRIu64"%s%"PRIu64"%s", base_url, qual,
                bandwidth, frag, start_time, end) < 0 )
    {
       return NULL;
    }
    free( url_template );
    return url;
}
