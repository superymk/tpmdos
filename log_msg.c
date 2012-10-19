#include "log_msg.h"
#include "log.h"

void FormatMsg(int log_type, char* msg)
{
    int cur_run_type = GetCurrentRunType();

    switch(cur_run_type)
    {
        case L_TEST:
            if(log_type == LOG_GENERAL)
            {
                char        type_str[LOG_MSG_LENGTH] = {0};
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                
                TranslateRunType(GetCurrentRunType(), type_str);
                sprintf(msg, "Result: run_type: %s, total_test: %llu, total_localrun: %llu, perf_result: %llu\n",
                    type_str,
                    (long long unsigned int)test_counter, 
                    (long long unsigned int)total_localrun,
                    (long long unsigned int)GetPerf(0)
                );
            }
            else if(log_type == LOG_GNUPLOT)
            {
                const char* timestamp = GetTimeStamp();
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                // For the L_TEST, the output data format:
                // #   (Date/Time)         (Total Run)    (Run for this type)      (Elapsed Time (us))
                sprintf(msg, "\t%s\t%llu\t%llu\t%llu\n", 
                        timestamp, 
                        (long long unsigned int)test_counter, 
                        (long long unsigned int)total_localrun,
                        (long long unsigned int)GetPerf(0)
                );
            }
            break;
        
        case WRITE_40BYTES:
           if(log_type == LOG_GENERAL)
            {
                char        type_str[LOG_MSG_LENGTH] = {0};
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                
                TranslateRunType(GetCurrentRunType(), type_str);
                sprintf(msg, "Result: run_type: %s, total_test: %llu, total_localrun: %llu, WRITE_NVWRITE_PERF: %llu, \
                        WRITE_ATTRIB_PERF: %llu, WRITE_POLICY_PERF: %llu, WRITE_KERNEL_PERF: %llu, WRITE_KERNEL_TIMEOUT_JIFFIES: %llu\n",
                    type_str,
                    (long long unsigned int)test_counter, 
                    (long long unsigned int)total_localrun,
                    (long long unsigned int)GetPerf(WRITE_NVWRITE_PERF),
                    (long long unsigned int)GetPerf(WRITE_ATTRIB_PERF),
                    (long long unsigned int)GetPerf(WRITE_POLICY_PERF),
                    (long long unsigned int)GetPerf(WRITE_KERNEL_PERF),
                    (long long unsigned int)GetPerf(WRITE_KERNEL_TIMEOUT_JIFFIES)
                );
            }
            else if(log_type == LOG_GNUPLOT)
            {
                const char* timestamp = GetTimeStamp();
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                // For the L_TEST, the output data format:
                // #   (Date/Time)         (Total Run)    (Run for this type)      (WRITE_NVWRITE_PERF (us))
                sprintf(msg, "\t%s\t%llu\t%llu\t%llu\t%llu\t%llu\t%llu\t%llu\n", 
                        timestamp, 
                        (long long unsigned int)test_counter, 
                        (long long unsigned int)total_localrun,
                        (long long unsigned int)GetPerf(WRITE_NVWRITE_PERF),
                        (long long unsigned int)GetPerf(WRITE_ATTRIB_PERF),
                        (long long unsigned int)GetPerf(WRITE_POLICY_PERF),
                        (long long unsigned int)GetPerf(WRITE_KERNEL_PERF),
                        (long long unsigned int)GetPerf(WRITE_KERNEL_TIMEOUT_JIFFIES)
                );
            }
            break;
            
        case READ_40BYTES:
            if(log_type == LOG_GENERAL)
            {
                char        type_str[LOG_MSG_LENGTH] = {0};
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                
                TranslateRunType(GetCurrentRunType(), type_str);
                sprintf(msg, "Result: run_type: %s, total_test: %llu, total_localrun: %llu, , READ_NVREAD_PERF: %llu, READ_ATTRIB_PERF: %llu\n",
                    type_str,
                    (long long unsigned int)test_counter, 
                    (long long unsigned int)total_localrun,
                    (long long unsigned int)GetPerf(READ_NVREAD_PERF),
                    (long long unsigned int)GetPerf(READ_ATTRIB_PERF)
                );
            }
            else if(log_type == LOG_GNUPLOT)
            {
                const char* timestamp = GetTimeStamp();
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                // For the L_TEST, the output data format:
                // #   (Date/Time)         (Total Run)    (Run for this type)      (READ_NVREAD_PERF (us))
                sprintf(msg, "\t%s\t%llu\t%llu\t%llu\t%llu\n", 
                        timestamp, 
                        (long long unsigned int)test_counter, 
                        (long long unsigned int)total_localrun,
                        (long long unsigned int)GetPerf(READ_NVREAD_PERF),
                        (long long unsigned int)GetPerf(READ_ATTRIB_PERF)
                );
            }
            break;
            
        case DEFINE_NVRAM:
            if(log_type == LOG_GENERAL)
            {
                char        type_str[LOG_MSG_LENGTH] = {0};
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                
                TranslateRunType(GetCurrentRunType(), type_str);
                sprintf(msg, "Result: run_type: %s, total_test: %llu, total_localrun: %llu, DEFINE_NVDEFINE_PERF: %llu, DEFINE_ATTRIB_PERF: %llu, DEFINE_POLICY_PERF: %llu\n\n",
                    type_str,
                    (long long unsigned int)test_counter, 
                    (long long unsigned int)total_localrun,
                    (long long unsigned int)GetPerf(DEFINE_NVDEFINE_PERF),
                    (long long unsigned int)GetPerf(DEFINE_ATTRIB_PERF),
                    (long long unsigned int)GetPerf(DEFINE_POLICY_PERF)
                );
            }
            else if(log_type == LOG_GNUPLOT)
            {
                const char* timestamp = GetTimeStamp();
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                // For the L_TEST, the output data format:
                // #   (Date/Time)         (Total Run)    (Run for this type)      (DEFINE_NVDEFINE_PERF (us))
                sprintf(msg, "\t%s\t%llu\t%llu\t%llu\t%llu\t%llu\n", 
                        timestamp, 
                        (long long unsigned int)test_counter, 
                        (long long unsigned int)total_localrun,
                        (long long unsigned int)GetPerf(DEFINE_NVDEFINE_PERF),
                        (long long unsigned int)GetPerf(DEFINE_ATTRIB_PERF),
                        (long long unsigned int)GetPerf(DEFINE_POLICY_PERF)
                );
            }
            break;
            
        default:
            PRINT("Error Reading Current Run Type\n");
            FATAL_ERROR();
    }
}