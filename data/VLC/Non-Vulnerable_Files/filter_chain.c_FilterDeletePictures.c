}
static void FilterDeletePictures( filter_t *filter, picture_t *picture )
{
    while( picture )
    {
        picture_t *next = picture->p_next;
        filter_DeletePicture( filter, picture );
        picture = next;
    }
}
