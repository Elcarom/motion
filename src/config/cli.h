#pragma once

namespace motion::config {

  // Entry point for `motion config <command> [options]`. Returns a process
  // exit code. Pure CLI helper; does not start Application or mutate live config.
  int runCli(int argc, char* argv[]);

} // namespace motion::config
