 *****************************************************************************/
static void VlcPictureToIplImage( filter_t* p_filter, picture_t* p_in )
{
    int planes = p_in->i_planes;    //num input video planes
    // input video size
    CvSize sz = cvSize(abs(p_in->format.i_width), abs(p_in->format.i_height));
    video_format_t fmt_out;
    filter_sys_t* p_sys = p_filter->p_sys;
    memset( &fmt_out, 0, sizeof(video_format_t) );
    //do scale / color conversion according to p_sys config
    if ((p_sys->f_scale != 1) || (p_sys->i_internal_chroma != CINPUT))
    {
        fmt_out = p_in->format;
        //calc the scaled video size
        fmt_out.i_width = p_in->format.i_width * p_sys->f_scale;
        fmt_out.i_height = p_in->format.i_height * p_sys->f_scale;
        if (p_sys->i_internal_chroma == RGB)
        {
            //rgb2 gives 3 separate planes, this gives 1 interleaved plane
            //rv24 gives is about 20% faster but gives r&b the wrong way round
            //and I can't think of an easy way to fix this
            fmt_out.i_chroma = VLC_CODEC_RGB24;
        }
        else if (p_sys->i_internal_chroma == GREY)
        {
            //take the I (gray) plane (video seems to commonly be in this fmt so usually the
            //conversion does nothing)
            fmt_out.i_chroma = VLC_CODEC_I420;
        }
        //convert from the input image
        p_sys->p_proc_image = image_Convert( p_sys->p_image, p_in,
                                     &(p_in->format), &fmt_out );
        if (!p_sys->p_proc_image)
        {
            msg_Err(p_filter, "can't convert (unsupported formats?), aborting...");
            return;
        }
        p_sys->p_to_be_freed = p_sys->p_proc_image;    //remember this so we can free it later
    }
    else    //((p_sys->f_scale != 1) || (p_sys->i_internal_chroma != CINPUT))
    {
        // In theory, you could use the input image without conversion,
        // but it seems to cause weird picture effects (like repeated
        // image filtering) and picture leaking.
        p_sys->p_proc_image = filter_NewPicture( p_filter ); //p_in
        picture_Copy( p_sys->p_proc_image, p_in );
        p_sys->p_to_be_freed = p_sys->p_proc_image;
    }
    //Convert to the IplImage array that is to be processed.
    //If there are multiple planes in p_sys->p_proc_image, then 1 IplImage
    //is created for each plane.
    planes = p_sys->p_proc_image->i_planes;
    p_sys->i_cv_image_size = planes;
    for( int i = 0; i < planes; i++ )
    {
        sz = cvSize(abs(p_sys->p_proc_image->p[i].i_visible_pitch /
            p_sys->p_proc_image->p[i].i_pixel_pitch),
            abs(p_sys->p_proc_image->p[i].i_visible_lines));
        p_sys->p_cv_image[i] = cvCreateImageHeader(sz, IPL_DEPTH_8U,
            p_sys->p_proc_image->p[i].i_pixel_pitch);
        cvSetData( p_sys->p_cv_image[i],
            (char*)(p_sys->p_proc_image->p[i].p_pixels), p_sys->p_proc_image->p[i].i_pitch );
    }
    //Hack the above opencv image array into a picture_t so that it can be sent to
    //another video filter
    p_sys->hacked_pic.i_planes = planes;
    p_sys->hacked_pic.format.i_chroma = fmt_out.i_chroma;
#ifndef NDEBUG
    msg_Dbg( p_filter, "VlcPictureToIplImageRgb() completed" );
#endif
}
