// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#include "LogStream.h"
#include <iomanip>
#include <unistd.h>

Log::LogStream::LogStream() noexcept: _pid(getpid()), _sign(0), _stream(nullptr), _mutex(nullptr)
{
}

Log::LogStream::LogStream(char sign, std::ostream &stream, std::shared_ptr<std::mutex> mutex):
        _pid(getpid()), _sign(sign), _stream(&stream), _mutex(std::move(mutex))
{
}

void Log::LogStream::printStr(size_t indent, const std::string &tag, const std::string &msg) const
{
    if (_stream == nullptr)
        return;
    size_t spos = 0;
    size_t epos = 0;
    while (epos != std::string::npos)
    {
        epos = msg.find('\n', epos + 1);
        println(indent, tag, msg.substr(spos, epos - spos));
        spos = epos + 1;
    }
}

void Log::LogStream::setStream(std::ostream &stream, std::shared_ptr<std::mutex> mutex)
{
    _stream = &stream;
    _mutex = std::move(mutex);
}

std::ostream &Log::LogStream::getStream() const
{
    return *_stream;
}

void Log::LogStream::setSign(char sign)
{
    _sign = sign;
}

std::shared_ptr<std::mutex> Log::LogStream::getMutex() const
{
    return _mutex;
}

void Log::LogStream::disable()
{
    _stream = nullptr;
    _mutex = nullptr;
}

std::ostream &Log::LogStream::putTime() const
{
    auto chronotime = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(chronotime);
    tm ltime{};
    *_stream << std::put_time(localtime_r(&time, &ltime), "%m-%d %T") << "." << std::setfill('0') << std::setw(9)
             << std::chrono::time_point_cast<std::chrono::nanoseconds>(chronotime).time_since_epoch().count()%1000000000;
    return *_stream;
}

std::ostream &Log::LogStream::putIndent(size_t N) const
{

    for(size_t i = 0; i < N; ++i)
    {
        *_stream<<"    ";
    }

    return *_stream;
}

void Log::LogStream::updatePID()
{
    _pid = getpid();
}

void Log::LogStream::printer<std::string>::operator()
        (const LogStream &stream, size_t indent, const std::string &tag, const std::string &msg)
{
    stream.printStr(indent, tag, msg);
}


void Log::LogStream::printer<char*>::operator()
        (const LogStream &stream, size_t indent, const std::string &scope, const char* msg)
{
    stream.printStr(indent, scope, msg);
}

void Log::LogStream::printer<const char*>::operator()
        (const LogStream &stream, size_t indent, const std::string &scope, const char* msg)
{
    stream.printStr(indent, scope, msg);
}
