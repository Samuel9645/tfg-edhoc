/**
 * @file
 *
 * @brief Simple integration test for the standalone EDHOC client and server
 * binaries.
 *
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 *
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unity.h>

static void execute_handshake_test(const char* server_port_arguments,
                                   const char* server_supporter_arguments,
                                   const char* client_argument_format) {
  const pid_t server_pid = fork();

  if (server_pid == 0) {
    // --- CHILD: Ejecuta el servidor con los argumentos parametrizados ---
    execl(SERVER_PATH, "server", "-p", server_port_arguments, "-s",
          server_supporter_arguments, NULL);
    exit(EXIT_FAILURE);
  }

  // --- PARENT ---
  sleep(1);

  // Construye el comando del cliente dinámicamente
  char client_cmd[512];
  snprintf(client_cmd, sizeof(client_cmd), "\"%s\" %s", CLIENT_PATH,
           client_argument_format);

  const int client_status = system(client_cmd);

  // Limpieza del proceso servidor
  kill(server_pid, SIGTERM);
  waitpid(server_pid, NULL, 0);

  // Validaciones del test (Unity Framework)
  if (WIFEXITED(client_status)) {
    TEST_ASSERT_EQUAL(0, WEXITSTATUS(client_status));
  } else {
    TEST_FAIL_MESSAGE("Client crashed or was signaled");
  }
}

void test_negotiation_suite_2_handshake(void) {
  execute_handshake_test("2", "2", "-p 0 -s 0 2 localhost");
}

void test_negotiation_suite_0_handshake(void) {
  execute_handshake_test("0", "0", "-p 2 -s 2 0 localhost");
}