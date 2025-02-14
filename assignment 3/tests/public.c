#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include <check.h>

#include "../signals.h"

START_TEST (MIN_sig_2_msg)
{
  printf ("\n======================================\n");
  printf ("MIN sig_2_msg\n");
  printf ("This test checks sig2msg maps the messages correctly\n");
  size_t length = 0;
  char *msg = sig2msg (SIGSEGV, &length);
  ck_assert_int_eq ((int)length, 8);
  ck_assert_str_eq (msg, "SEGFAULT");

}
END_TEST

/* testing name-to-signal translation */
START_TEST (MIN_getsig)
{
  printf ("\n======================================\n");
  printf ("MIN getsig\n");
  printf ("This test checks getsig maps the messages correctly\n");
  ck_assert_int_eq (getsig ("INT"), 2);
}
END_TEST

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_add_test (tc_public, MIN_sig_2_msg);
  tcase_add_test (tc_public, MIN_getsig);
  suite_add_tcase (s, tc_public);
}

