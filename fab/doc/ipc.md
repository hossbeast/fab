#fab ipc

For a project, the fab system consists of four processes, viz.

 - fab : parses command-line arguments and marshals them for faba/fabd
 - fabd : daemon that maintains all state pertaining to the project and carries out most logic
 - fabw : daemon that watches fabd/faba to ensure continuity in the case of crashes
 - faba : daemon that runs with user credentials to carry out filesystem tasks

This document describes the ipc that takes place among this set of processes

## process hierarchy
 user:user      fab
 fabsys:fabsys  fabw
 fabsys:fabsys  |-- fabd
 user:user      fabw
 user:user      |-- faba

## signal exchange

Quoting from man 2 kill :

  For a process to have permission to send a signal it must either be
  privileged (under Linux: have the CAP_KILL capability), or the real
  or effective user ID of the sending process must equal the real or
  saved set-user-ID of the target process. 

Example process credentials for fab

                faba      fabd
  real       >> todd      root   <<
  effective  >> todd      fabsys
  saved         fabsys    fabsys <<

In order to facilitate signal exchange, these programs require fabsys:fabsys ownership
and ug+s permissions. fabd runs with fabsys credentials at all times, thus it is always
eligible to receive signals. fab and faba temporarily assume fabsys credentials before
sending a signal to fabd, and maintain them while awaiting the response signal

## signal flow
FABSIG_START    : fab -> fabd    new command
FABSIG_DONE     : fabd -> faba   command complete
FABSIG_DONE     : faba -> fab    command complete

FABSIG_BPSTART  : fabd -> faba   buildplan stage ready to be executed
FABSIG_BPGOOD   : faba -> fabd   buildplan stage executed to completion
FABSIG_BPBAD    : faba -> fabd   buildplan stage failed 

FOO             : faba -> fabd   filesystem changes ; re-interpret fabfiles
FOO             : faba -> fabd   filesystem changes ; re-execute the buildplan
FOO             : faba -> fabd   fabfile changes ; fabd must be re-executed

## per project
```
/var/run/fab/4213918793                                   # <fabipcdir>/<hash>
|-- [drwxrwxrwx]  fabd
|   |-- [-rw-rw-rw-]  pgid                                # ascii
|   |-- [-rw-rw-rw-]  exit                                # binary (last exit status)
|   |-- [-rw-rw-rw-]  error                               # binary (last error code)
|   `-- [-rw-rw-rw-]  lock                                # empty
|-- [drwxrwxrwx]  fab
|   |-- [lrwxrwxrwx]  cwd -> /home/ec2-user/fab
|   |-- [-rw-rw-rw-]  pid                                 # ascii
|   |-- [lrwxrwxrwx]  out -> /dev/pts/2
|   |-- [-rw-rw-rw-]  lock                                # empty
|   `-- [lrwxrwxrwx]  err -> /dev/pts/2
|-- [lrwxrwxrwx]  dsc -> /var/tmp/fab/pid/2796/dsc
|-- [lrwxrwxrwx]  bp -> /var/tmp/fab/pid/10451/bp
|-- [-rw-rw-rw-]  logs                                    # ascii
|-- [lrwxrwxrwx]  fabfile -> /home/ec2-user/fab/fabfile
`-- [-rw-rw-rw-]  args                                    # binary
```

## per fabd instance
```
/var/tmp/fab/pid/2796                                     # <fabtmpdir>/pid/<pid-of-fabd>
├── [drwxrwxrwx]  dsc
│   └── [drwxrwxrwx]  12
│       ├── [-rwxrwxrwx]  cmd                             # ascii
│       ├── [-rw-rw-rw-]  err                             # ascii
│       └── [-rw-rw-rw-]  out                             # ascii
└── [-rw-rw-rw-]  stamp                                   # empty (last-modified-time has meaning)
```

## per faba instance
```
/var/tmp/fab/pid/10451                                    # <fabtmpdir>/pid/<pid-of-faba>
├── [drwxrwxrwx]  bp
│   ├── [drwxrwxrwx]  7
│   │   ├── [drwxrwxrwx]  2
│   │   │   ├── [-rwxrwxrwx]  cmd                         # ascii
│   │   │   ├── [-rw-rw-rw-]  err                         # ascii
│   │   │   ├── [-rw-rw-rw-]  exit                        # binary (exit status)
│   │   │   ├── [-rw-rw-rw-]  out                         # ascii
│   │   │   └── [-rw-rw-rw-]  prod                        # ascii, null-delimited
│   ├── [-rw-rw-rw-]  commands                            # binary (sum of */*)
│   └── [-rw-rw-rw-]  stages                              # binary (sum of *)
└── [-rw-rw-rw-]  stamp                                   # empty (last-modified-time has meaning)
```
