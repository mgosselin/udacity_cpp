#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string mem_entry, mem_value;
  float mem_total, mem_free;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> mem_entry >> mem_value) {
        if (mem_entry == "MemTotal:") { mem_total = stof(mem_value); }
        if (mem_entry == "MemFree:") { mem_free = stof(mem_value); }
      }
    }
  }
  return (mem_total - mem_free) / mem_total;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string uptime_total, uptime_idle;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime_total >> uptime_idle;
  }
  return stof(uptime_total);
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string val;
  long active_clock_ticks;
  vector<string> process_stats;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> val || !linestream.eof()) {
      process_stats.push_back(val);
    }
  }
  active_clock_ticks = stol(process_stats[14]) + stol(process_stats[15]) + stol(process_stats[16]) + stol(process_stats[17]);
  return active_clock_ticks;
}

// DONE: Read and return the number of jiffies corresponding to the start of a process
long LinuxParser::StartJiffies(int pid) {
  string line;
  string val;
  long start_clock_ticks;
  vector<string> process_stats;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> val || !linestream.eof()) {
      process_stats.push_back(val);
    }
  }
  start_clock_ticks = stol(process_stats[21]);
  return start_clock_ticks;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_utilization = LinuxParser::CpuUtilization();
  return stol(cpu_utilization[kUser_]) +
         stol(cpu_utilization[kNice_]) +
         stol(cpu_utilization[kSystem_]) +
         stol(cpu_utilization[kIRQ_]) +
         stol(cpu_utilization[kSoftIRQ_]) +
         stol(cpu_utilization[kSteal_]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_utilization = LinuxParser::CpuUtilization();
  return stol(cpu_utilization[kIdle_]) +
         stol(cpu_utilization[kIOwait_]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string entry_name, val;
  vector<string> utilization;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> entry_name;
      if (entry_name == "cpu") {
        while (linestream >> val || !linestream.eof()) {
          utilization.push_back(val);
        }
      }
    }
  }
  return utilization;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string first_entry, second_entry;
  float proc_total;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> first_entry >> second_entry) {
        if (first_entry == "processes") {
          proc_total = stof(second_entry);
          return proc_total;
        }
      }
    }
  }
  return proc_total;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string first_entry, second_entry;
  float proc_running;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> first_entry >> second_entry) {
        if (first_entry == "procs_running") {
          proc_running = stof(second_entry);
          return proc_running;
        }
      }
    }
  }
  return proc_running;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  string command;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> command;
  }
  return command;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string ram;
  string first_entry, second_entry;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) +  kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> first_entry >> second_entry) {
        if (first_entry == "VmSize:") {
          ram = to_string( stof(second_entry) / 1000.0 );
          return ram;
        }
      }
    }
  }
  return ram;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string uid;
  string first_entry, second_entry;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) +  kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> first_entry >> second_entry) {
        if (first_entry == "uid:") {
          uid = second_entry;
          return uid;
        }
      }
    }
  }
  return uid;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line;
  string first_entry, second_entry, third_entry;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> first_entry >> second_entry >> third_entry) {
        if (third_entry == uid) {
          return first_entry;
        }
      }
    }
  }
  return first_entry;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string val;
  long uptime_clock_ticks;
  vector<string> process_stats;
  std::ifstream filestream(kProcDirectory + '/' + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> val || !linestream.eof()) {
      process_stats.push_back(val);
    }
  }
  uptime_clock_ticks = stol(process_stats[22]);
  return uptime_clock_ticks / sysconf(_SC_CLK_TCK);
}
