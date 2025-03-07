#define _POSIX_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "../client.h"
#include "../server.h"

#include "helper.h"

START_TEST (PART_get_record)
{
  printf ("\n======================================\n");
  printf ("PARTIAL CREDIT: get_record (public.c)\n");
  char *req = build_mq ("request1");
  char *resp = build_mq ("response1");
  ck_assert (req != NULL);
  ck_assert (resp != NULL);
  pid_t server = test_server ("P1", req, resp);
  ids_resp_t *response = NULL;
  bool success = get_record ("../data/int.txt", req, resp, &response);
  kill (server, SIGUSR1);
  unlink ("P1");
  mq_unlink (req);
  mq_unlink (resp);
  ck_assert (success);
  ck_assert_int_eq (response->mode, 0100640);
  ck_assert_int_eq (response->size, 4);
  ck_assert_str_eq (response->cksum, "2330645186");
  free (response);
}
END_TEST

// Test that accept_string accepts a basic "hello" string
START_TEST (PART_check_record_good)
{
  printf ("\n======================================\n");
  printf ("PARTIAL CREDIT: check_record_good (public.c)\n");
  char *filename = "../data/int.txt";
  ids_resp_t resp;
  resp.type = RESPONSE;
  resp.mode = 0100640;
  resp.size = 4;
  memset (&resp.cksum, 0, 12);
  strncpy ((char *)&resp.cksum, "2330645186", 11);
  ck_assert (check_record (filename, &resp));
}
END_TEST

START_TEST (MIN_check_record_bad)
{
  printf ("\n======================================\n");
  printf ("MINIMUM CREDIT: check_record_bad (public.c)\n");
  char *filename = "../data/int.txt";
  ids_resp_t resp;
  resp.type = RESPONSE;
  resp.mode = 0100440;
  resp.size = 4;
  memset (&resp.cksum, 0, 12);
  strncpy ((char *)&resp.cksum, "2330645186", 11);
  ck_assert (! check_record (filename, &resp));
}
END_TEST

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_add_test (tc_public, PART_get_record);
  tcase_add_test (tc_public, PART_check_record_good);
  tcase_add_test (tc_public, MIN_check_record_bad);
  suite_add_tcase (s, tc_public);
}

