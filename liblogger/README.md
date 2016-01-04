# liblogger

An application using liblogger configures a set of output streams, each of
which is equipped with a set of filters. Components of the application issue
logging requests that are tagged with one or more categories. The filters for a
stream determine, based on its categories, which log messages are output to
that stream. The application provides a default set of streams and filters,
which can be overridden with command-line parameters, and reconfigured
dynamically at runtime.

# logging requests

Before issuing a logging request, an application component calls
logger_register to obtains ids for the categories it will use. Any component
that registers a category with a given name receives the same id.

# logging options

Log messages have associated options. For example, the log message may be
colored red, or have a timestamp. For any given log message, the value of these
settings is derived according to the following rules.

For each option, in increasing order of precedence:

1. the category
2. the log site
3. the stream

When a log message belongs to multiple categories, the category setting for a
given option comes from the category with the highest precedence where that
option is set. Categories with a lower id (i.e. that are listed first in
logs.c) have higher precedence.

# rationale

* liblogger has a hard dependency on libxapi ; the default, before initialization completes, is to log all messages to stderr
* every component that uses liblogger must explicitly call logger_register during setup
