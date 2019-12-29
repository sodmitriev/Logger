// Copyright 2019 Sviatoslav Dmitriev
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "Logger.h"

std::string get(size_t pos, std::stringstream& sstream)
{
    auto str = sstream.str();
    std::string::size_type spos = 0;
    std::string::size_type epos = str.find_first_of(" \n", spos);
    for(size_t i = 0; i < pos; ++i)
    {
        if(epos == std::string::npos || epos == str.length() - 1)
        {
            return "";
        }
        spos = str.find_first_not_of(" \n", epos);
        epos = str.find_first_of(" \n", spos);
    }
    if(epos != std::string::npos)
    {
        return str.substr(spos, epos - spos);
    }
    else
    {
        return str.substr(spos);
    }
}

std::string strThID()
{
    std::stringstream idconv;
    idconv << std::this_thread::get_id();
    return idconv.str();
}

TEST_CASE("LoggerTest")
{

    char sign = GENERATE(take(1, random('!', '~')));
    size_t scopelen = GENERATE(take(1, random(size_t(1), size_t(255))));
    size_t msglen = GENERATE(take(1, random(size_t(1), size_t(255))));
    auto scopevec = GENERATE_COPY(take(1, chunk(scopelen, random('!', '~'))));
    auto msgvec = GENERATE_COPY(take(1, chunk(msglen, random('!', '~'))));
    std::string scope(scopevec.begin(), scopevec.end());
    std::string msg(msgvec.begin(), msgvec.end());
    Log::LogLevel level = Log::LogLevel(GENERATE(take(1, random(size_t(0), Log::levels - 1))));

    std::stringstream out;
    std::stringstream out1;

    SECTION("DefaultConstructDestructLogStream","[log-stream]")
    {
        Log::LogStream *lstr = nullptr;
        REQUIRE_NOTHROW(lstr = new(std::nothrow) Log::LogStream);
        REQUIRE_NOTHROW(delete lstr);
    }

    SECTION("ConstructLogStream", "[log-stream]")
    {
        Log::LogStream *lstr = nullptr;
        REQUIRE_NOTHROW(lstr = new(std::nothrow) Log::LogStream(sign, std::cout, nullptr));
        REQUIRE_NOTHROW(delete lstr);
    }

    SECTION("PrintToLogStream","[log-stream]")
    {
        Log::LogStream lstr(sign, out, nullptr);
        REQUIRE_NOTHROW(Log::LogStream::printer<std::string>()(lstr, 0, scope, msg));
        REQUIRE(get(2, out) == std::to_string(getpid()));
        REQUIRE(get(3, out) == strThID());
        REQUIRE(get(4, out) == std::string(1, sign));
        REQUIRE(get(5, out) == scope + ':');
        REQUIRE(get(6, out) == msg);
    }

    SECTION("UseMutexLogStream","[log-stream]")
    {
        Log::LogStream lstr(sign, out, std::make_shared<std::mutex>());
        REQUIRE_NOTHROW(Log::LogStream::printer<std::string>()(lstr, 0, scope, msg));
        REQUIRE(get(2, out) == std::to_string(getpid()));
        REQUIRE(get(3, out) == strThID());
        REQUIRE(get(4, out) == std::string(1, sign));
        REQUIRE(get(5, out) == scope + ':');
        REQUIRE(get(6, out) == msg);
    }

    SECTION("ChangeSign","[log-stream]")
    {
        Log::LogStream lstr(sign, out, nullptr);
        REQUIRE_NOTHROW(lstr.setSign(sign));
        REQUIRE_NOTHROW(Log::LogStream::printer<std::string>()(lstr, 0, scope, msg));
        REQUIRE(get(2, out) == std::to_string(getpid()));
        REQUIRE(get(3, out) == strThID());
        REQUIRE(get(4, out) == std::string(1, sign));
        REQUIRE(get(5, out) == scope + ':');
        REQUIRE(get(6, out) == msg);
    }

    SECTION("ChangeStream","[log-stream]")
    {
        Log::LogStream lstr(sign, out, nullptr);
        REQUIRE_NOTHROW(lstr.setStream(out1, nullptr));
        REQUIRE_NOTHROW(lstr.setSign(sign));
        REQUIRE_NOTHROW(Log::LogStream::printer<std::string>()(lstr, 0, scope, msg));
        REQUIRE(get(2, out1) == std::to_string(getpid()));
        REQUIRE(get(3, out1) == strThID());
        REQUIRE(get(4, out1) == std::string(1, sign));
        REQUIRE(get(5, out1) == scope + ':');
        REQUIRE(get(6, out1) == msg);
    }

    SECTION("Disable","[log-stream]")
    {
        Log::LogStream lstr(sign, out, nullptr);
        REQUIRE_NOTHROW(lstr.disable());
        REQUIRE_NOTHROW(lstr.setSign(sign));
        REQUIRE_NOTHROW(Log::LogStream::printer<std::string>()(lstr, 0, scope, msg));
        REQUIRE(get(0, out) == "");
    }

    SECTION("ConstructDestructLogger", "[logger]")
    {
        Log::Logger *logger;
        REQUIRE_NOTHROW(logger = new(std::nothrow) Log::Logger);
        REQUIRE_NOTHROW(delete logger);
    }

    SECTION("SetStreamLogger", "[logger]")
    {
        Log::Logger logger;
        REQUIRE_NOTHROW(logger.setStream(level, out));
    }

    SECTION("PrintLogger", "[logger]")
    {
        Log::Logger logger;
        for(unsigned int i = 0; i < Log::levels; ++i)
        {
            logger.setStream(static_cast<Log::LogLevel >(i), out);
        }
        REQUIRE_NOTHROW(logger.print(level, 0, __func__, msg));

        auto cc_ptr = "const char * is a string literal in c++";

        constexpr size_t arrSize = 63;
        char c_arr[arrSize + 1];
        c_arr[arrSize] = '\0';
        memcpy(c_arr, msg.data(), arrSize);
        REQUIRE_NOTHROW(logger.print(level, 0, __func__, cc_ptr));
        REQUIRE_NOTHROW(logger.print(level, 0, __func__, "const char * is a string literal in c++"));
        REQUIRE_NOTHROW(logger.print(level, 0, __func__, c_arr));
        char sign;
        switch(level)
        {
            case Log::Info:
                sign = 'I';
                break;
            case Log::Verbose:
                sign = 'V';
                break;
            case Log::Warning:
                sign = 'W';
                break;
            case Log::Error:
                sign = 'E';
                break;
            case Log::Assert:
                sign = 'A';
                break;
            case Log::Debug:
                sign = 'D';
                break;
            default:
                sign = ' ';
                break;
        }
        REQUIRE(get(2, out) == std::to_string(getpid()));
        REQUIRE(get(3, out) == strThID());
        REQUIRE(get(4, out) == std::string(1, sign));
        REQUIRE(get(5, out) == std::string(__func__) + ':');
        REQUIRE(get(6, out) == msg);

        REQUIRE(get(9, out) == std::to_string(getpid()));
        REQUIRE(get(10, out) == strThID());
        REQUIRE(get(11, out) == std::string(1, sign));
        REQUIRE(get(12, out) == std::string(__func__) + ':');
        REQUIRE(get(13, out) == "const");
        REQUIRE(get(14, out) == "char");
        REQUIRE(get(15, out) == "*");
        REQUIRE(get(16, out) == "is");
        REQUIRE(get(17, out) == "a");
        REQUIRE(get(18, out) == "string");
        REQUIRE(get(19, out) == "literal");
        REQUIRE(get(20, out) == "in");
        REQUIRE(get(21, out) == "c++");

        REQUIRE(get(24, out) == std::to_string(getpid()));
        REQUIRE(get(25, out) == strThID());
        REQUIRE(get(26, out) == std::string(1, sign));
        REQUIRE(get(27, out) == std::string(__func__) + ':');
        REQUIRE(get(28, out) == "const");
        REQUIRE(get(29, out) == "char");
        REQUIRE(get(30, out) == "*");
        REQUIRE(get(31, out) == "is");
        REQUIRE(get(32, out) == "a");
        REQUIRE(get(33, out) == "string");
        REQUIRE(get(34, out) == "literal");
        REQUIRE(get(35, out) == "in");
        REQUIRE(get(36, out) == "c++");

        REQUIRE(get(39, out) == std::to_string(getpid()));
        REQUIRE(get(40, out) == strThID());
        REQUIRE(get(41, out) == std::string(1, sign));
        REQUIRE(get(42, out) == std::string(__func__) + ':');
        REQUIRE(get(43, out) == msg.substr(0, arrSize));
    }

    SECTION("DisableLevelLogger", "[logger]")
    {
        Log::Logger logger;
        for(unsigned int i = 0; i < Log::levels; ++i)
        {
            logger.setStream(static_cast<Log::LogLevel >(i), out);
        }
        REQUIRE_NOTHROW(logger.disableLevel(level));
        REQUIRE_NOTHROW(logger.print(level, 0, __func__, msg.data()));
        REQUIRE(get(0, out) == "");
        REQUIRE_NOTHROW(logger.setStream(level,out));
    }

    SECTION("DefaultLogger", "[logger]")
    {
        for(unsigned int i = 0; i < Log::levels; ++i)
        {
            Log::defaultLog.setStream(static_cast<Log::LogLevel >(i), out);
        }
        REQUIRE_NOTHROW(Log::defaultLog.print(level, 0, __func__, msg.data()));
        char sign;
        switch(level)
        {
            case Log::Info:
                sign = 'I';
                break;
            case Log::Verbose:
                sign = 'V';
                break;
            case Log::Warning:
                sign = 'W';
                break;
            case Log::Error:
                sign = 'E';
                break;
            case Log::Assert:
                sign = 'A';
                break;
            case Log::Debug:
                sign = 'D';
                break;
            default:
                sign = ' ';
                break;
        }
        REQUIRE(get(2, out) == std::to_string(getpid()));
        REQUIRE(get(3, out) == strThID());
        REQUIRE(get(4, out) == std::string(1, sign));
        REQUIRE(get(5, out) == std::string(__func__) + ':');
        REQUIRE(get(6, out) == msg);
    }
}
