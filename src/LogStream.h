// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#ifndef LOGGER_LOGSTREAM_H
#define LOGGER_LOGSTREAM_H

#include <ostream>
#include <memory>
#include <mutex>
#include <thread>

namespace Log
{
    /*!
     * Logging stream class
     */
    class LogStream
    {
    public:


        /*!
         * Structure used to print variables of different types into log
         * @tparam MsgT Variable type
         */
        template <typename MsgT>
        struct printer
        {


            /*!
             * Prints message to the log
             * @param stream Log stream to print the message into
             * @param tag Message scope
             * @param msg Message
             */
            void operator()(const LogStream& stream, size_t indent, const std::string &tag, const MsgT &msg);
        };
    private:
        __pid_t _pid;
        char _sign;
        std::ostream * _stream;
        std::shared_ptr<std::mutex> _mutex;
        std::ostream & putTime() const;
        std::ostream & putIndent(size_t N) const;
    public:


        /*!
         * Constructor
         */
        LogStream() noexcept;


        /*!
         * Constructor
         * @param sign Character that specifies this stream
         * @param stream Pointer to stream to print messages into
         */
        LogStream(char sign, std::ostream &stream, std::shared_ptr<std::mutex> mutex);


        /*!
         * Print a single line into the stream
         * @param tag Scope name
         * @param line Line to be printed
         */
        template <typename MsgT>
        void println(size_t indent, const std::string &tag, const MsgT &line) const;


        /*!
         * Print a string into the stream
         * @param tag Scope name
         * @param msg Message
         */
        void printStr(size_t indent, const std::string &tag, const std::string &msg) const;


        /*!
         * Set output stream
         * @param stream Output stream
         * @param mutex Mutex to use for specified output stream
         */
        void setStream(std::ostream &stream, std::shared_ptr<std::mutex> mutex);


        /*!
         * Get output stream
         * @note Unidentified behaviour if stream is disabled
         * @return Output stream
         */
        std::ostream &getStream() const;


        /*!
         * Set sign
         * @param sign Character that specifies this stream
         */
        void setSign(char sign);


        /*!
         * Get mutex used for this stream
         * @return Mutex used in this stream
         */
        std::shared_ptr<std::mutex> getMutex() const;

        /*!
         * Check if stream is enabled
         * @return true if enabled, false otherwise
         */
        inline bool enabled() const
        { return _stream != nullptr; }

        /*!
         * Disable this thread
         */
        void disable();

        /*!
         * Updates logger's buffered PID value
         */
        void updatePID();
    };
}

template<typename MsgT>
void Log::LogStream::println(size_t indent, const std::string &tag, const MsgT &line) const
{
    if (_mutex != nullptr)
        _mutex->lock();
    putTime();
    (*_stream) << "  " << _pid << "  " << std::this_thread::get_id() << " ";
    (*_stream) << _sign << " " << tag << ": ";
    putIndent(indent);
    (*_stream)<< line << std::endl;
    if (_mutex != nullptr)
        _mutex->unlock();
}

template <>
struct Log::LogStream::printer<std::string>
{
    void operator()(const LogStream& stream, size_t indent, const std::string &tag, const std::string &msg);
};

template <>
struct Log::LogStream::printer<char*>
{
    void operator()(const LogStream& stream, size_t indent, const std::string &tag, const char* msg);
};

template <>
struct Log::LogStream::printer<const char*>
{
    void operator()(const LogStream& stream, size_t indent, const std::string &tag, const char* msg);
};

template <size_t N>
struct Log::LogStream::printer<char[N]>
{
    void operator()(const LogStream& stream, size_t indent, const std::string &tag, const char* msg)
    {
        stream.printStr(indent, tag, msg);
    }
};

template<typename MsgT>
void Log::LogStream::printer<MsgT>::operator()
        (const LogStream &stream, size_t indent, const std::string &tag, const MsgT &msg)
{
    stream.printStr(indent, tag, std::to_string(msg));
}

#endif //LOGGER_LOGSTREAM_H
