#pragma once

namespace motion::ipc {

  // Entry point for `motion msg <command> [args...]`. Returns a process exit
  // code. Forwards the command to the running instance over the IPC socket.
  int runCli(int argc, char* argv[]);

} // namespace motion::ipc
