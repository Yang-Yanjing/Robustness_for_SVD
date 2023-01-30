 */
char *mstrtime( char *psz_buffer, mtime_t date )
{
    static const mtime_t ll1000 = 1000, ll60 = 60, ll24 = 24;
    snprintf( psz_buffer, MSTRTIME_MAX_SIZE, "%02d:%02d:%02d-%03d.%03d",
             (int) (date / (ll1000 * ll1000 * ll60 * ll60) % ll24),
             (int) (date / (ll1000 * ll1000 * ll60) % ll60),
             (int) (date / (ll1000 * ll1000) % ll60),
             (int) (date / ll1000 % ll1000),
             (int) (date % ll1000) );
    return( psz_buffer );
}
