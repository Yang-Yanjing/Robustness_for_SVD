 *****************************************************************************/
static picture_t* Filter( filter_t* p_filter, picture_t* p_pic )
{
    picture_t* p_outpic = filter_NewPicture( p_filter );
    if( p_outpic == NULL ) {
        msg_Err( p_filter, "couldn't get a p_outpic!" );
        picture_Release( p_pic );
        return NULL;
    }
    video_format_t fmt_out;
    // Make a copy if we want to show the original input
    if (p_filter->p_sys->i_wrapper_output == VINPUT)
        picture_Copy( p_outpic, p_pic );
    VlcPictureToIplImage( p_filter, p_pic );
    // Pass the image (as a pointer to the first IplImage*) to the
    // internal OpenCV filter for processing.
    p_filter->p_sys->p_opencv->pf_video_filter( p_filter->p_sys->p_opencv, (picture_t*)&(p_filter->p_sys->p_cv_image[0]) );
    if(p_filter->p_sys->i_wrapper_output == PROCESSED) {
        // Processed video
        if( (p_filter->p_sys->p_proc_image) &&
            (p_filter->p_sys->p_proc_image->i_planes > 0) &&
            (p_filter->p_sys->i_internal_chroma != CINPUT) ) {
            //p_filter->p_sys->p_proc_image->format.i_chroma = VLC_CODEC_RGB24;
            memset( &fmt_out, 0, sizeof(video_format_t) );
            fmt_out = p_pic->format;
            //picture_Release( p_outpic );
            /*
             * We have to copy out the image from image_Convert(), otherwise
             * you leak pictures for some reason:
             * main video output error: pictures leaked, trying to workaround
             */
            picture_t* p_outpic_tmp = image_Convert(
                        p_filter->p_sys->p_image,
                        p_filter->p_sys->p_proc_image,
                        &(p_filter->p_sys->p_proc_image->format),
                        &fmt_out );
            picture_CopyPixels( p_outpic, p_outpic_tmp );
            CopyInfoAndRelease( p_outpic, p_outpic_tmp );
        } else if( p_filter->p_sys->i_internal_chroma == CINPUT ) {
            picture_CopyPixels( p_outpic, p_filter->p_sys->p_proc_image );
            picture_CopyProperties( p_outpic, p_filter->p_sys->p_proc_image );
        }
    }
    ReleaseImages( p_filter );
    picture_Release( p_pic );
#ifndef NDEBUG
    msg_Dbg( p_filter, "Filter() done" );
#endif
    if( p_filter->p_sys->i_wrapper_output != NONE ) {
        return p_outpic;
    } else { // NONE
        picture_Release( p_outpic );
        return NULL;
    }
}
