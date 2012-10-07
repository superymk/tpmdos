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
                uint64_t    perf_result = GetPerf(PERF_RESULT);
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                
                TranslateRunType(GetCurrentRunType(), type_str);
                sprintf(msg, "Result: run_type: %s, total_test: %llu, total_localrun: %llu, perf_result: %llu\n",
                    type_str,
                    (long long unsigned int)test_counter, 
                    (long long unsigned int)total_localrun,
                    (long long unsigned int)perf_result
                );
            }
            else if(log_type == LOG_GNUPLOT)
            {
                const char* timestamp = GetTimeStamp();
                uint64_t    perf_result = GetPerf(PERF_RESULT);
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                // For the L_TEST, the output data format:
                // #   (Date/Time)         (Total Run)    (Run for this type)      (Elapsed Time (us))
                sprintf(msg, "\t%s\t%llu\t%llu\t%llu\n", 
                        timestamp, 
                        (long long unsigned int)test_counter, 
                        (long long unsigned int)total_localrun,
                        (long long unsigned int)perf_result
                );
            }
            break;
        
        case WRITE_40BYTES:
           if(log_type == LOG_GENERAL)
            {
                char        type_str[LOG_MSG_LENGTH] = {0};
                uint64_t    perf_result = GetPerf(PERF_RESULT);
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                
                TranslateRunType(GetCurrentRunType(), type_str);
                sprintf(msg, "Result: run_type: %s, total_test: %llu, total_localrun: %llu, perf_result: %llu\n",
                    type_str,
                    (long long unsigned int)test_counter, 
                    (long long unsigned int)total_localrun,
                    (long long unsigned int)perf_result
                );
            }
            else if(log_type == LOG_GNUPLOT)
            {
                const char* timestamp = GetTimeStamp();
                uint64_t    perf_result = GetPerf(PERF_RESULT);
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                // For the L_TEST, the output data format:
                // #   (Date/Time)         (Total Run)    (Run for this type)      (Elapsed Time (us))
                sprintf(msg, "\t%s\t%llu\t%llu\t%llu\n", 
                        timestamp, 
                        (long long unsigned int)test_counter, 
                        (long long unsigned int)total_localrun,
                        (long long unsigned int)perf_result
                );
            }
            break;
            
        case READ_40BYTES:
            if(log_type == LOG_GENERAL)
            {
                char        type_str[LOG_MSG_LENGTH] = {0};
                uint64_t    perf_result = GetPerf(PERF_RESULT);
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                
                TranslateRunType(GetCurrentRunType(), type_str);
                sprintf(msg, "Result: run_type: %s, total_test: %llu, total_localrun: %llu, perf_result: %llu\n",
                    type_str,
                    (long long unsigned int)test_counter, 
                    (long long unsigned int)total_localrun,
                    (long long unsigned int)perf_result
                );
            }
            else if(log_type == LOG_GNUPLOT)
            {
                const char* timestamp = GetTimeStamp();
                uint64_t    perf_result = GetPerf(PERF_RESULT);
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                // For the L_TEST, the output data format:
                // #   (Date/Time)         (Total Run)    (Run for this type)      (Elapsed Time (us))
                sprintf(msg, "\t%s\t%llu\t%llu\t%llu\n", 
                        timestamp, 
                        (long long unsigned int)test_counter, 
                        (long long unsigned int)total_localrun,
                        (long long unsigned int)perf_result
                );
            }
            break;
            
        case DEFINE_NVRAM:
            if(log_type == LOG_GENERAL)
            {
                char        type_str[LOG_MSG_LENGTH] = {0};
                uint64_t    perf_result = GetPerf(PERF_RESULT);
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                
                TranslateRunType(GetCurrentRunType(), type_str);
                sprintf(msg, "Result: run_type: %s, total_test: %llu, total_localrun: %llu, perf_result: %llu\n",
                    type_str,
                    (long long unsigned int)test_counter, 
                    (long long unsigned int)total_localrun,
                    (long long unsigned int)perf_result
                );
            }
            else if(log_type == LOG_GNUPLOT)
            {
                const char* timestamp = GetTimeStamp();
                uint64_t    perf_result = GetPerf(PERF_RESULT);
                uint64_t    test_counter = GetGlobalTestCounter();
                uint64_t    total_localrun = GetMetaValue(GetCurrentRunType());
                // For the L_TEST, the output data format:
                // #   (Date/Time)         (Total Run)    (Run for this type)      (Elapsed Time (us))
                sprintf(msg, "\t%s\t%llu\t%llu\t%llu\n", 
                        timestamp, 
                        (long long unsigned int)test_counter, 
                        (long long unsigned int)total_localrun,
                        (long long unsigned int)perf_result
                );
            }
            break;
            
        default:
            PRINT("Error Reading Current Run Type\n");
            FATAL_ERROR();
    }
}