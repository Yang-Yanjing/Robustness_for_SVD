}
void json_value_free (json_value * value)
{
   json_value * cur_value;
   if (!value)
      return;
   value->parent = 0;
   while (value)
   {
      switch (value->type)
      {
         case json_array:
            if (!value->u.array.length)
            {
               free (value->u.array.values);
               break;
            }
            value = value->u.array.values [-- value->u.array.length];
            continue;
         case json_object:
            if (!value->u.object.length)
            {
               free (value->u.object.values);
               break;
            }
            value = value->u.object.values [-- value->u.object.length].value;
            continue;
         case json_string:
            free (value->u.string.ptr);
            break;
         default:
            break;
      };
      cur_value = value;
      value = value->parent;
      free (cur_value);
   }
}
