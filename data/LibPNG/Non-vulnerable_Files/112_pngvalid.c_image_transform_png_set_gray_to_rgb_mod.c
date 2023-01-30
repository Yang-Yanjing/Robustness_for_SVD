static void
image_transform_png_set_gray_to_rgb_mod(PNG_CONST image_transform *this,
    image_pixel *that, png_const_structp pp,
    PNG_CONST transform_display *display)
{
   




   if ((that->colour_type & PNG_COLOR_MASK_COLOR) == 0 && that->have_tRNS)
      image_pixel_add_alpha(that, &display->this);
   
   if (that->colour_type == PNG_COLOR_TYPE_GRAY)
   {
      
      if (that->bit_depth < 8)
         that->sample_depth = that->bit_depth = 8;
      


      that->colour_type = PNG_COLOR_TYPE_RGB;
   }
   else if (that->colour_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      that->colour_type = PNG_COLOR_TYPE_RGB_ALPHA;
   this->next->mod(this->next, that, pp, display);
}