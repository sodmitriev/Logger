// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <vector>
#include "LogStream.h"


//! Mark compile-time enabled log levels
#ifndef LOGGER_LOG_INFO_ENABLED
#define LOGGER_LOG_INFO_ENABLED 1
#endif

#ifndef LOGGER_LOG_VERBOSE_ENABLED
#define LOGGER_LOG_VERBOSE_ENABLED 1
#endif

#ifndef LOGGER_LOG_WARNING_ENABLED
#define LOGGER_LOG_WARNING_ENABLED 1
#endif

#ifndef LOGGER_LOG_ERROR_ENABLED
#define LOGGER_LOG_ERROR_ENABLED 1
#endif

#ifndef LOGGER_LOG_WTF_ENABLED
#define LOGGER_LOG_WTF_ENABLED 1
#endif

#ifndef LOGGER_LOG_DEBUG_ENABLED
#ifdef NDEBUG
#define LOGGER_LOG_DEBUG_ENABLED 0
#else
#define LOGGER_LOG_DEBUG_ENABLED 1
#endif
#endif


#ifndef LOGGER_LOG_DEBUG_RESTRICTED
#define LOGGER_LOG_DEBUG_RESTRICTED 0
#endif

#ifndef LOGGER_LOG_DEBUG_ALLOWED
#define LOGGER_LOG_DEBUG_ALLOWED 0
#endif

//! Print message into info stream
#if LOGGER_LOG_INFO_ENABLED
#define LOG_INFO(msg) Log::defaultLog.print(Log::Info, 0, __FUNCTION__, msg)
#define LOG_INFO_TAG(msg, tag) Log::defaultLog.print(Log::Info, 0, tag, msg)
#else
#define LOG_INFO(msg) ((void)0)
#define LOG_INFO_TAG(msg, tag) ((void)0)
#endif


//!Print message into verbose stream
#if LOGGER_LOG_VERBOSE_ENABLED
#define LOG_VERBOSE(msg) Log::defaultLog.print(Log::Verbose, 0, __FUNCTION__, msg)
#define LOG_VERBOSE_TAG(msg, tag) Log::defaultLog.print(Log::Verbose, 0, tag, msg)
#else
#define LOG_VERBOSE(msg) ((void)0)
#define LOG_VERBOSE_TAG(msg, tag) ((void)0)
#endif


//!Print message into warning stream
#if LOGGER_LOG_WARNING_ENABLED
#define LOG_WARNING(msg) Log::defaultLog.print(Log::Warning, 0, __FUNCTION__, msg)
#define LOG_WARNING_TAG(msg, tag) Log::defaultLog.print(Log::Warning, 0, tag, msg)
#else
#define LOG_WARNING(msg) ((void)0)
#define LOG_WARNING_TAG(msg, tag) ((void)0)
#endif


//!Print message into error stream
#if LOGGER_LOG_ERROR_ENABLED
#define LOG_ERROR(msg) Log::defaultLog.print(Log::Error, 0, __PRETTY_FUNCTION__, msg)
#define LOG_ERROR_TAG(msg, tag) Log::defaultLog.print(Log::Error, 0, tag, msg)
#else
#define LOG_ERROR(msg) ((void)0)
#define LOG_ERROR_TAG(msg, tag) ((void)0)
#endif

//!Print message into assert stream
#if LOGGER_LOG_WTF_ENABLED
#define LOG_WTF(msg) Log::defaultLog.print(Log::Assert, 0, __PRETTY_FUNCTION__, msg)
#define LOG_WTF_TAG(msg, tag) Log::defaultLog.print(Log::Assert, 0, tag, msg)
#else
#define LOG_WTF(msg) ((void)0)
#define LOG_WTF_TAG(msg, tag) ((void)0)
#endif

//!Print message into debug stream
#if LOGGER_LOG_DEBUG_ENABLED && (!LOGGER_LOG_DEBUG_RESTRICTED || LOGGER_LOG_DEBUG_ALLOWED)
#define LOG_DEBUG(msg) Log::defaultLog.print(Log::Debug, 0, __PRETTY_FUNCTION__, msg)
#define LOG_DEBUG_TAG(msg, tag) Log::defaultLog.print(Log::Debug, 0, tag, msg)
#else
#define LOG_DEBUG(msg) ((void)0)
#define LOG_DEBUG_TAG(msg, tag) ((void)0)
#endif

//! Disable a specific logger level on runtime
#define LOGGER_DISABLE_LEVEL(level) Log::defaultLog.disableLevel(level)

//! Set output stream for a specific logger level
#define LOGGER_SET_STREAM(level, stream) Log::defaultLog.setStream(level, stream)

//! Update cached pid after forking
#define LOGGER_UPDATE_PID() Log::defaultLog.updatePID()

namespace Log
{

    /*!
     * Amount of log levels
     */
    extern const size_t levels;

    /*!
     * Logging levels
     */
    enum LogLevel : size_t
    {
        Info = 0,
        Verbose = 1,
        Warning = 2,
        Error = 3,
        Assert = 4,
        Debug = 5,
    };


    /*!
     * Logger class
     */
    class Logger
    {
    private:
        std::vector<LogStream> _streams;
    public:


        /*!
         * Constructor
         */
        Logger() noexcept;


        /*!
         * Destructor
         */
        ~Logger() = default;


        /*!
         * Set output stream for a log level
         * @param level Log level
         * @param outStream New stream for the level
         */
        void setStream(LogLevel level, std::ostream &outStream);


        /*!
         * Disable a log level
         * @param level Log level to disable
         */
        void disableLevel(LogLevel level);


        /*!
         * Output messages to log
         * @param level Log level
         * @param tag Message tag
         * @param msg Message
         * @param args Other messages
         */
        template <typename MsgT, typename ... Args>
        void print(LogLevel level, size_t indent, const std::string &tag, const MsgT& msg, const Args& ... args)const;


        /*!
         * Output nothing into log
         * Used to build templates
         * @param level Log level
         * @param tag Message tag
         */
        void print(LogLevel level, size_t indent, const std::string &tag) const;


        /*!
         * Updates logger's buffered PID value
         */
        void updatePID();
    };

    extern Logger defaultLog;
}

template<typename MsgT, typename... Args>
void Log::Logger::print(Log::LogLevel level, size_t indent, const std::string &tag, const MsgT &msg, const Args &... args) const
{
    if (level < _streams.size() && _streams[level].enabled())
    {
        LogStream::printer<MsgT>()(_streams[level], indent, tag, msg);
        print(level, indent, tag, args...);
    }
}

#endif //LOGGER_LOGGER_H
