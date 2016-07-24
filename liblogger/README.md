# liblogger

An application using liblogger configures a set of output streams, each of which is equipped with a
set of filters. Components in the application issue logging requests that are associated with one or
more categories. For each logging request, the filters for a stream determine, based on the categories
associated with the request, whether that request is output to that stream.

The application provides a default set of streams and filters, which can be overridden with
command-line parameters, and reconfigured dynamically at runtime.

# initialization procedure

1. logger_load (n times)
2. logger_arguments_setup (from main)
3. logger_category_register (n times)
4. logger_stream_register (n times)
5. logger_finalize

Steps 3 and 4 may be repeated as necessary to add new categories and re-resolve filter expressions

# default request handling

A logging request is subject to default request handling when 1) there are no active streams, or 2)
it has no associated categories. Both of these cases arise when initialization has not been
completed, or was done incorrectly (for example, when component fails to call
logger_category_register, all of its log messages will have zero for the category ids).

By default, such requests are silently dropped. Thus, an application which does not itself link and
configure liblogger, but which links libraries that do use liblogger (and logger_category_register),
will not get any output from liblogger at all.

An applicaction that is using liblogger should change the default behavior to instead write all log
messages to stderr, by exporting and setting to nonzero an integer named g_logger_default_stderr.

# logging requests

At startup time, an application component calls logger_register to obtains ids for the categories it
will use. Any component that registers a category with a given name receives the same id.

# logging options

Log messages have associated options. For example, the log message may be colored red, or have a
timestamp. For any given log message, the value of these settings is derived according to the
following rules, in increasing order of precedence:

1. the category
2. the log site
3. the stream

When a log message belongs to multiple categories, the category setting for a given option comes
from the category with the highest precedence where that option is set. Categories with a lower id
(i.e. that are listed first in logs.c) have higher precedence.
