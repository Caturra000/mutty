#ifndef __MUTTY_LOG_H__
#define __MUTTY_LOG_H__
#include "Log.hpp"

#ifdef MUTTY_FLAG_DLOG_ENABLE
    #define LOG_DEBUG(...) DLOG_DEBUG_ALIGN(__VA_ARGS__)
    #define LOG_INFO(...)  DLOG_INFO_ALIGN(__VA_ARGS__)
    #define LOG_WARN(...)  DLOG_WARN_ALIGN(__VA_ARGS__)
    #define LOG_ERROR(...) DLOG_ERROR_ALIGN(__VA_ARGS__)
    #define LOG_WTF(...)   DLOG_WTF_ALIGN(__VA_ARGS__)
    namespace mutty { using dlog::Log; }
    namespace mutty { using dlog::IoVector; }
    namespace mutty { using dlog::filename; }
#else
    #define LOG_DEBUG(...) (void)0
    #define LOG_INFO(...)  (void)0
    #define LOG_WARN(...)  (void)0
    #define LOG_ERROR(...) (void)0
    #define LOG_WTF(...)   (void)0
    #define DLOG_CONF_PATH "log/null.conf"
#endif

#define MUTTY_LOG_DEBUG(...) LOG_DEBUG("[mutty]", __VA_ARGS__)
#define MUTTY_LOG_INFO(...)  LOG_INFO("[mutty]", __VA_ARGS__)
#define MUTTY_LOG_WARN(...)  LOG_WARN("[mutty]", __VA_ARGS__)
#define MUTTY_LOG_ERROR(...) LOG_ERROR("[mutty]", __VA_ARGS__)
#define MUTTY_LOG_WTF(...)   LOG_WTF("[mutty]", __VA_ARGS__)


#endif