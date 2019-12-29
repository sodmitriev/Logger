# Logger
Simple thread safe logging library for C++.

# Usage

You can link the logger library via CMake with the following line:
`target_link_libraries(yourLibrary Logger)`

Logger supports 6 configurable log levels:
- info
- verbose
- warning
- error
- assert
- debug

Info, verbose, warning and debug levels by default will output messages into std::cout. Error and assert levels by default will output messages into std::cerr. Custom output streams can assigned to log levels on runtime.

Debug logs will be disabled in release builds.

To write a message into any log level use one of the following macros:
- LOG_INFO(msg)
- LOG_INFO_TAG(msg, tag)
- LOG_VERBOSE(msg)
- LOG_VERBOSE_TAG(msg, tag)
- LOG_WARNING(msg)
- LOG_WARNING_TAG(msg, tag)
- LOG_ERROR(msg)
- LOG_ERROR_TAG(msg, tag)
- LOG_WTF(msg)
- LOG_WTF_TAG(msg, tag)
- LOG_DEBUG(msg)
- LOG_DEBUG_TAG(msg, tag)

Tagged macroses are used to provide a custom message tag, by default `__FUNCTION__` used as a tag on info, verbose and warning levels, and `__PRETTY_FUNCTION__` on debug, error and assert levels.

To disable any specific log level on runtime call `LOGGER_DISABLE_LEVEL(level)` macro.
To disable any specific log level on compile time define the following macro before including the logger header:
`LOGGER_<LOG_MACRO>_ENABLED 0`

To enable previously disabled:

`LOGGER_<LOG_MACRO>_ENABLED 1`

For example:

`LOGGER_LOG_DEBUG_ENABLED 0`

For testing purposes, debug level can be disabled per library. Set `LOGGER_LOG_DEBUG_RESTRICTED` macro to 1 to disable debug messages only in current library. Set `LOGGER_LOG_DEBUG_ALLOWED` to 1 to bypass this restriction in current library. `LOGGER_LOG_DEBUG_ENABLED` must be set to 1 for this feature to work.

To configure a level output stream call `LOGGER_SET_STREAM(level, stream)` macro. `stream` must represent a class that inherits std::ostream.

Lastly, if your programm forks, call `LOGGER_UPDATE_PID()` macro in the beginning of a forked process.

# Output format
Logger outputs messages in the following format:
`<time> <process id> <thread id> <log level> <message tag>: <message>`

# Custom type logging
To enable logging of a custom type, declare a specialization of the `Log::LogStream::printer` class, and implement the `operator()` method.

For example:

```c++
struct myStruct
{
    int val1;
    std::string val2;
};

template <>
struct Log::LogStream::printer<myStruct>
{
    void operator()(const LogStream& stream, size_t indent, const std::string &tag, const myStruct &msg)
    {
        stream.printStr(indent, tag, "myStruct:");
        ++indent;
        stream.printStr(indent, tag, "val1: " + std::to_string(val1));
        stream.printStr(indent, tag, "val2: " + val2);
        --indent;
    }
};
```

Use `stream.printStr()` to print multiline messages and `stream.println()` to print single line messages.

To print values with a custom priner specialization, call `Log::LogStream::printer<myType>()(stream, indent, tag, myValue)` 

Increase and decrease indent value to print your type with proper indentation.

# Separate logger objects
By default one global logger is created by the library. To use different loggers in different parts of your code, you may create additional objects of class `Log::Logger`. 
