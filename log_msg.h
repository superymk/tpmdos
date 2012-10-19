/**
 * log_msg.h -- Different perfcases require different formats, as well as perf 
 * slots. log_msg.h records these formats.
 */

#ifndef _LOG_MSG_H
#define _LOG_MSG_H

// The perf slots required by each perf test.

#define DEFINE_NVDEFINE_PERF            0
#define DEFINE_ATTRIB_PERF              1
#define DEFINE_POLICY_PERF              2

#define WRITE_NVWRITE_PERF              0
#define WRITE_ATTRIB_PERF               1
#define WRITE_POLICY_PERF               2
#define WRITE_KERNEL_PERF               3
#define WRITE_KERNEL_TIMEOUT_JIFFIES    4

#define READ_NVREAD_PERF                0
#define READ_ATTRIB_PERF                1


// Available Log Type
#define LOG_GENERAL     1
#define LOG_GNUPLOT     2

extern void FormatMsg(int log_type, char* msg);

#endif
