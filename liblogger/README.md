# liblogger

An application using liblogger issues logging requests that are tagged with one
or more categories. Some requests are ignored, and some result in the emission
of log messages. This behavior is governed by one or more filters.  The
application provides a default set of filters, which can be overridden with
command-line parameters. The filters can be changed dynamically at runtime.

Before issuing a logging request, a component in the application first calls
logger_register to obtains category ids for the logging categories it will use.
Components which register a category with a given name receive the same id.

# rationale

* liblogger has a hard dependency on libxapi ; the default, before initialization completes, is to log all messages to stderr
* every component that uses liblogger must explicitly call logger_register during setup
