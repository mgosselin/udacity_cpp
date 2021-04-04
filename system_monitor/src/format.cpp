#include <string>

#include "format.h"

using std::string;
using std::to_string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  string result_s_SS, result_s_MM, result_s_HH;
  int result_i_SS, result_i_MM, result_i_HH;
  // seconds
  result_i_SS = seconds % 60;
  if (result_i_SS < 10) {
    result_s_SS = "0" + to_string(result_i_SS);
  } else {
    result_s_SS = to_string(result_i_SS);
  }
  // minutes
  result_i_MM = ( (seconds - result_i_SS) / 60 ) % 60;
  if (result_i_MM < 10) {
    result_s_MM = "0" + to_string(result_i_MM);
  } else {
    result_s_MM = to_string(result_i_MM);
  }
  // hours
  result_i_HH = ( ( (seconds - result_i_SS) / 60 ) - result_i_MM ) / 60;
  if (result_i_HH < 10) {
    result_s_HH = "0" + to_string(result_i_HH);
  } else {
    result_s_HH = to_string(result_i_HH);
  }
  return result_s_HH + ":" + result_s_MM + ":" + result_s_SS;
}