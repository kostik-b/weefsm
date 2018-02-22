// Copyright Konstantin Bakanov 2018

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
  The purpose of this test is to build and test a way to reallocate an array of
  pointers.
*/


#define RESIZE_PTR_ARRAY(ELEM_TYPE, arr_ptr, prev_size, new_size) \
  ELEM_TYPE* temp_arr = calloc (new_size, sizeof (ELEM_TYPE));    \
  memcpy (temp_arr, arr_ptr, prev_size * sizeof (ELEM_TYPE));     \
  free (arr_ptr);                                                 \
  arr_ptr = temp_arr;

typedef struct my_struct1
{
  int m_a;
} my_struct1;

int main (int argc, char** argv)
{
  // here we have 10 ptrs to ptrs of the type my_struct1
  printf ("Allocating the array with elements of size %d\n", sizeof(my_struct1*));
  my_struct1** arr_ptr1 = calloc (10, sizeof(my_struct1*));
  arr_ptr1[0] = malloc (sizeof (my_struct1));
  arr_ptr1[0]->m_a = 13;
  arr_ptr1[9] = malloc (sizeof (my_struct1));
  arr_ptr1[9]->m_a = 14;

  // we now want a generic macro to resize that
  printf ("Resizing the array\n");
  RESIZE_PTR_ARRAY (my_struct1*, arr_ptr1, 10, 20);

  printf ("arr_ptr - 0 : %d\n", arr_ptr1[0]->m_a);
  printf ("arr_ptr - 9 : %d\n", arr_ptr1[9]->m_a);

  return 0;
}
