}
int playlist_Status( playlist_t * p_playlist )
{
    PL_ASSERT_LOCKED;
    return pl_priv(p_playlist)->status.i_status;
}
