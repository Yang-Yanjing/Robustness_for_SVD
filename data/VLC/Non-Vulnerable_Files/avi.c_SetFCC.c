static block_t *avi_HeaderCreateidx1( sout_mux_t * );
static void SetFCC( uint8_t *p, char *fcc )
{
    memcpy( p, fcc, 4 );
}
