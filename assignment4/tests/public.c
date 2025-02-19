#include <check.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../pipe.h"
#include "../map.h"

// Change this to true if you want to inspect the files produced in
// tests/inputs for debugging
#define DEBUG false

START_TEST (MIN_split_string)
{
  printf ("\n======================================\n");
  printf ("MIN_split_string\n");
  printf ("Splits \"ha ha\" into \"ha\"\n");

  char buffer[] = "ha ha";
  char *result = split_string (buffer);
  ck_assert_str_eq (result, "ha");
}
END_TEST

START_TEST (MIN_cksum)
{
  printf ("\n======================================\n");
  printf ("MIN_cksum\n");
  printf ("Gets cksum of data/f1.txt\n");

  char *result = get_cksum ("data/f1.txt");
  ck_assert_str_eq (result, "3015617425 6 data/f1.txt\n");
}
END_TEST

START_TEST (FULL_size)
{
  printf ("\n======================================\n");
  printf ("FULL_size\n");
  printf ("Gets the file size of data/f1.txt\n");

  int fd = open ("data/f1.txt", O_RDONLY);
  size_t size = get_file_size (fd);
  close (fd);
  ck_assert_int_eq (6, size);
}
END_TEST

START_TEST (FULL_file_name)
{
  printf ("\n======================================\n");
  printf ("FULL_file_name\n");
  printf ("Gets a line number from a null-byte separated array\n");

  char buffer[] = "data/f1.txt data/f2.txt data/f3.txt";
  buffer[11] = '\0';
  buffer[23] = '\0';

  char *result = get_file_name (buffer, 0, 36);
  ck_assert_str_eq (result, "data/f1.txt");
  result = get_file_name (buffer, 1, 36);
  ck_assert_str_eq (result, "data/f2.txt");
  result = get_file_name (buffer, 2, 36);
  ck_assert_str_eq (result, "data/f3.txt");
}
END_TEST

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_add_test (tc_public, MIN_split_string);
  tcase_add_test (tc_public, MIN_cksum);
  tcase_add_test (tc_public, FULL_size);
  tcase_add_test (tc_public, FULL_file_name);
  suite_add_tcase (s, tc_public);
}

