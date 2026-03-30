#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unity.h>

// TODO: Maybe use another technique to test this instead of cmake

void test_standalone_binaries_handshake(void) {
  pid_t server_pid = fork();

  if (server_pid == 0) {
    // --- CHILD: Use the absolute path provided by CMake ---
    execl(SERVER_PATH, "server", (char*)NULL);

    exit(EXIT_FAILURE);
  } else {
    // --- PARENT ---
    sleep(1);

    // Construct the system command using the absolute path
    char client_cmd[512];
    snprintf(client_cmd, sizeof(client_cmd), "%s", CLIENT_PATH);

    int client_status = system(client_cmd);

    kill(server_pid, SIGTERM);
    waitpid(server_pid, NULL, 0);

    TEST_ASSERT_EQUAL(0, WEXITSTATUS(client_status));
  }
}