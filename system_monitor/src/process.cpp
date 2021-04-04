#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid) {
  pid_ = pid;
}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const {
  // this calculation from:
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  float elapsed_time_s = (float)LinuxParser::UpTime() - (float)LinuxParser::StartJiffies(pid_) / (float)sysconf(_SC_CLK_TCK);
  float active_time_s = (float)LinuxParser::ActiveJiffies(pid_) / (float)sysconf(_SC_CLK_TCK);
  return active_time_s / elapsed_time_s;
}

// DONE: Return the command that generated this process
string Process::Command() {
  return LinuxParser::Command(pid_);
}

// DONE: Return this process's memory utilization
string Process::Ram() {
  return LinuxParser::Ram(pid_);
}

// DONE: Return the user (name) that generated this process
string Process::User() {
  return LinuxParser::User(pid_);
}

// DONE: Return the age of this process (in seconds)
long Process::UpTime() {
  return LinuxParser::UpTime(pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }