// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#include "Logger.h"
#include <iostream>

Log::Logger Log::defaultLog;

#if LOGGER_LOG_DEBUG_ENABLED
const size_t Log::levels = 6;
#else
const size_t Log::levels = 5;
#endif

Log::Logger::Logger() noexcept
{
    std::shared_ptr<std::mutex> coutMutex = std::make_shared<std::mutex>();
    std::shared_ptr<std::mutex> cerrMutex = std::make_shared<std::mutex>();
    _streams.resize(levels);
    _streams[0] = LogStream('I', std::cout, coutMutex);
    _streams[1] = LogStream('V', std::cout, coutMutex);
    _streams[2] = LogStream('W', std::cout, coutMutex);
    _streams[3] = LogStream('E', std::cerr, cerrMutex);
    _streams[4] = LogStream('A', std::cerr, cerrMutex);
#if LOGGER_LOG_DEBUG_ENABLED
    _streams[5] = LogStream('D', std::cout, coutMutex);
#endif
}

void Log::Logger::setStream(Log::LogLevel level, std::ostream &outStream)
{
    if(level < levels)
    {
        for(size_t i = 0; i < levels; ++i)
        {
            if(i != level && _streams[i].enabled() && &_streams[i].getStream() == &outStream)
            {
                _streams[level].setStream(outStream, _streams[i].getMutex());
                return;
            }
        }
        _streams[level].setStream(outStream, std::make_shared<std::mutex>());
    }
}

void Log::Logger::disableLevel(Log::LogLevel level)
{
    if(level < levels)
        _streams[level].disable();
}

void Log::Logger::print(Log::LogLevel, size_t, const std::string&) const
{
}

void Log::Logger::updatePID()
{
    for(auto & i : _streams)
    {
        i.updatePID();
    }
}
