#include "processor.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  // used this stack overflow post:
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux/23376195#23376195
  long curr_idle = LinuxParser::IdleJiffies();
  long curr_total = LinuxParser::Jiffies();
  long diff_idle = curr_idle - prev_idle_;
  long diff_total = curr_total - prev_total_;
  prev_idle_ = curr_idle;
  prev_total_ = curr_total;
  return (float)(diff_total - diff_idle) / (float)diff_total;
}