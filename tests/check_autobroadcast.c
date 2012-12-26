#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <check.h>
#include "../src/autobroadcast.h"

#define SMALL_BUFFER_SIZE 2000
#define RECEIVE_BUFFER_SIZE 65536
#define PORT 2345

void
setup (void)
{
}

void
teardown (void)
{
}

START_TEST (test_small_payload)
{
    /* set up listenling socket */
    struct sockaddr_in si_me, si_other;
    int s, i, bytes, slen=sizeof(si_other);
    char recv_buf[RECEIVE_BUFFER_SIZE], send_buf[SMALL_BUFFER_SIZE];
    
    fail_if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1, "listening socket");
    
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    fail_if (bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) == -1,"bind");

    /* send random data */
    for(i = 0; i < SMALL_BUFFER_SIZE; i++) {
      send_buf[i] = 66;
    }
    send_buf[0] = 65;
    send_buf[SMALL_BUFFER_SIZE-1] = 67;
    fail_unless(broadcast(PORT, send_buf, SMALL_BUFFER_SIZE) == SMALL_BUFFER_SIZE); 
    fail_unless(broadcast(PORT, send_buf, SMALL_BUFFER_SIZE) == SMALL_BUFFER_SIZE); 

    /* check if data was received */
    fail_if ((bytes = recvfrom(s, recv_buf, RECEIVE_BUFFER_SIZE, MSG_DONTWAIT, (struct sockaddr *) &si_other, &slen))==-1, "receiving data");
    close(s);
    printf("received %d bytes\n", bytes);
    fail_unless(bytes == SMALL_BUFFER_SIZE, "received data length mismatch");
    fail_unless(strncmp(send_buf, recv_buf, SMALL_BUFFER_SIZE) == 0, "data not identical");
    
    return;
}
END_TEST

Suite *
autobroadcast_suite (void)
{
  Suite *s = suite_create ("Autobroadcast");

  /* Core test case */
  TCase *tc_core = tcase_create ("Core");
  tcase_add_checked_fixture (tc_core, setup, teardown);
  tcase_add_test (tc_core, test_small_payload);
  suite_add_tcase (s, tc_core);

  return s;
}

int
main (void)
{
  int number_failed;
  Suite *s = autobroadcast_suite ();
  SRunner *sr = srunner_create (s);
  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
