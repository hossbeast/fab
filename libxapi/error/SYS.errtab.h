#ifndef _SYS_ERRTAB_H
#define _SYS_ERRTAB_H
#include "xapi.h"
#define ETABLE_SYS \
_E(1, EPERM, "Operation not permitted") \
_E(2, ENOENT, "No such file or directory") \
_E(3, ESRCH, "No such process") \
_E(4, EINTR, "Interrupted system call") \
_E(5, EIO, "I/O error") \
_E(6, ENXIO, "No such device or address") \
_E(8, ENOEXEC, "Exec format error") \
_E(9, EBADF, "Bad file number") \
_E(10, ECHILD, "No child processes") \
_E(11, EAGAIN, "Try again") \
_E(12, ENOMEM, "Out of memory") \
_E(13, EACCES, "Permission denied") \
_E(14, EFAULT, "Bad address") \
_E(15, ENOTBLK, "Block device required") \
_E(16, EBUSY, "Device or resource busy") \
_E(17, EEXIST, "File exists") \
_E(18, EXDEV, "Cross-device link") \
_E(19, ENODEV, "No such device") \
_E(20, ENOTDIR, "Not a directory") \
_E(21, EISDIR, "Is a directory") \
_E(22, EINVAL, "Invalid argument") \
_E(23, ENFILE, "File table overflow") \
_E(24, EMFILE, "Too many open files") \
_E(25, ENOTTY, "Not a typewriter") \
_E(26, ETXTBSY, "Text file busy") \
_E(27, EFBIG, "File too large") \
_E(28, ENOSPC, "No space left on device") \
_E(29, ESPIPE, "Illegal seek") \
_E(30, EROFS, "Read-only file system") \
_E(31, EMLINK, "Too many links") \
_E(32, EPIPE, "Broken pipe") \
_E(33, EDOM, "Math argument out of domain of func") \
_E(34, ERANGE, "Math result not representable") \
_E(35, EDEADLK, "Resource deadlock would occur") \
_E(36, ENAMETOOLONG, "File name too long") \
_E(37, ENOLCK, "No record locks available") \
_E(38, ENOSYS, "Function not implemented") \
_E(39, ENOTEMPTY, "Directory not empty") \
_E(40, ELOOP, "Too many symbolic links encountered") \
_E(42, ENOMSG, "No message of desired type") \
_E(43, EIDRM, "Identifier removed") \
_E(44, ECHRNG, "Channel number out of range") \
_E(48, ELNRNG, "Link number out of range") \
_E(49, EUNATCH, "Protocol driver not attached") \
_E(50, ENOCSI, "No CSI structure available") \
_E(52, EBADE, "Invalid exchange") \
_E(53, EBADR, "Invalid request descriptor") \
_E(54, EXFULL, "Exchange full") \
_E(55, ENOANO, "No anode") \
_E(56, EBADRQC, "Invalid request code") \
_E(57, EBADSLT, "Invalid slot") \
_E(59, EBFONT, "Bad font file format") \
_E(60, ENOSTR, "Device not a stream") \
_E(61, ENODATA, "No data available") \
_E(62, ETIME, "Timer expired") \
_E(63, ENOSR, "Out of streams resources") \
_E(64, ENONET, "Machine is not on the network") \
_E(65, ENOPKG, "Package not installed") \
_E(66, EREMOTE, "Object is remote") \
_E(67, ENOLINK, "Link has been severed") \
_E(68, EADV, "Advertise error") \
_E(69, ESRMNT, "Srmount error") \
_E(70, ECOMM, "Communication error on send") \
_E(71, EPROTO, "Protocol error") \
_E(72, EMULTIHOP, "Multihop attempted") \
_E(73, EDOTDOT, "RFS specific error") \
_E(74, EBADMSG, "Not a data message") \
_E(75, EOVERFLOW, "Value too large for defined data type") \
_E(76, ENOTUNIQ, "Name not unique on network") \
_E(77, EBADFD, "File descriptor in bad state") \
_E(78, EREMCHG, "Remote address changed") \
_E(79, ELIBACC, "Can not access a needed shared library") \
_E(80, ELIBBAD, "Accessing a corrupted shared library") \
_E(81, ELIBSCN, ".lib section in a.out corrupted") \
_E(82, ELIBMAX, "Attempting to link in too many shared libraries") \
_E(83, ELIBEXEC, "Cannot exec a shared library directly") \
_E(84, EILSEQ, "Illegal byte sequence") \
_E(85, ERESTART, "Interrupted system call should be restarted") \
_E(86, ESTRPIPE, "Streams pipe error") \
_E(87, EUSERS, "Too many users") \
_E(88, ENOTSOCK, "Socket operation on non-socket") \
_E(89, EDESTADDRREQ, "Destination address required") \
_E(90, EMSGSIZE, "Message too long") \
_E(91, EPROTOTYPE, "Protocol wrong type for socket") \
_E(92, ENOPROTOOPT, "Protocol not available") \
_E(93, EPROTONOSUPPORT, "Protocol not supported") \
_E(94, ESOCKTNOSUPPORT, "Socket type not supported") \
_E(95, EOPNOTSUPP, "Operation not supported on transport endpoint") \
_E(96, EPFNOSUPPORT, "Protocol family not supported") \
_E(97, EAFNOSUPPORT, "Address family not supported by protocol") \
_E(98, EADDRINUSE, "Address already in use") \
_E(99, EADDRNOTAVAIL, "Cannot assign requested address") \
_E(100, ENETDOWN, "Network is down") \
_E(101, ENETUNREACH, "Network is unreachable") \
_E(102, ENETRESET, "Network dropped connection because of reset") \
_E(103, ECONNABORTED, "Software caused connection abort") \
_E(104, ECONNRESET, "Connection reset by peer") \
_E(105, ENOBUFS, "No buffer space available") \
_E(106, EISCONN, "Transport endpoint is already connected") \
_E(107, ENOTCONN, "Transport endpoint is not connected") \
_E(108, ESHUTDOWN, "Cannot send after transport endpoint shutdown") \
_E(109, ETOOMANYREFS, "Too many references: cannot splice") \
_E(110, ETIMEDOUT, "Connection timed out") \
_E(111, ECONNREFUSED, "Connection refused") \
_E(112, EHOSTDOWN, "Host is down") \
_E(113, EHOSTUNREACH, "No route to host") \
_E(114, EALREADY, "Operation already in progress") \
_E(115, EINPROGRESS, "Operation now in progress") \
_E(116, ESTALE, "Stale file handle") \
_E(117, EUCLEAN, "Structure needs cleaning") \
_E(118, ENOTNAM, "Not a XENIX named type file") \
_E(119, ENAVAIL, "No XENIX semaphores available") \
_E(120, EISNAM, "Is a named type file") \
_E(121, EREMOTEIO, "Remote I/O error") \
_E(122, EDQUOT, "Quota exceeded") \
_E(123, ENOMEDIUM, "No medium found") \
_E(124, EMEDIUMTYPE, "Wrong medium type") \
_E(125, ECANCELED, "Operation Canceled") \
_E(126, ENOKEY, "Required key not available") \
_E(127, EKEYEXPIRED, "Key has expired") \
_E(128, EKEYREVOKED, "Key has been revoked") \
_E(129, EKEYREJECTED, "Key was rejected by service") \
_E(130, EOWNERDEAD, "Owner died") \
_E(131, ENOTRECOVERABLE, "State not recoverable") \
_E(132, ERFKILL, "Operation not possible due to RF-kill") \
_E(133, EHWPOISON, "Memory page has hardware error") \

enum {
#define _E(a, b, c) SYS_ ## b = a,
ETABLE_SYS
#undef _E
};
#define ERRMIN_SYS 1
#define ERRMAX_SYS 133
extern etable * perrtab_SYS;
#endif
