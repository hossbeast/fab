#ifndef _KERNEL_ERRTAB_H
#define _KERNEL_ERRTAB_H
#include "xapi.h"
#define ETABLE_KERNEL \
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
_E(38, ENOSYS, "Invalid system call number") \
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

#if XAPI_STACKTRACE
#define KERNEL_EPERM perrtab_KERNEL->v[1 - ERRMIN_KERNEL].exit
#define KERNEL_ENOENT perrtab_KERNEL->v[2 - ERRMIN_KERNEL].exit
#define KERNEL_ESRCH perrtab_KERNEL->v[3 - ERRMIN_KERNEL].exit
#define KERNEL_EINTR perrtab_KERNEL->v[4 - ERRMIN_KERNEL].exit
#define KERNEL_EIO perrtab_KERNEL->v[5 - ERRMIN_KERNEL].exit
#define KERNEL_ENXIO perrtab_KERNEL->v[6 - ERRMIN_KERNEL].exit
#define KERNEL_ENOEXEC perrtab_KERNEL->v[8 - ERRMIN_KERNEL].exit
#define KERNEL_EBADF perrtab_KERNEL->v[9 - ERRMIN_KERNEL].exit
#define KERNEL_ECHILD perrtab_KERNEL->v[10 - ERRMIN_KERNEL].exit
#define KERNEL_EAGAIN perrtab_KERNEL->v[11 - ERRMIN_KERNEL].exit
#define KERNEL_ENOMEM perrtab_KERNEL->v[12 - ERRMIN_KERNEL].exit
#define KERNEL_EACCES perrtab_KERNEL->v[13 - ERRMIN_KERNEL].exit
#define KERNEL_EFAULT perrtab_KERNEL->v[14 - ERRMIN_KERNEL].exit
#define KERNEL_ENOTBLK perrtab_KERNEL->v[15 - ERRMIN_KERNEL].exit
#define KERNEL_EBUSY perrtab_KERNEL->v[16 - ERRMIN_KERNEL].exit
#define KERNEL_EEXIST perrtab_KERNEL->v[17 - ERRMIN_KERNEL].exit
#define KERNEL_EXDEV perrtab_KERNEL->v[18 - ERRMIN_KERNEL].exit
#define KERNEL_ENODEV perrtab_KERNEL->v[19 - ERRMIN_KERNEL].exit
#define KERNEL_ENOTDIR perrtab_KERNEL->v[20 - ERRMIN_KERNEL].exit
#define KERNEL_EISDIR perrtab_KERNEL->v[21 - ERRMIN_KERNEL].exit
#define KERNEL_EINVAL perrtab_KERNEL->v[22 - ERRMIN_KERNEL].exit
#define KERNEL_ENFILE perrtab_KERNEL->v[23 - ERRMIN_KERNEL].exit
#define KERNEL_EMFILE perrtab_KERNEL->v[24 - ERRMIN_KERNEL].exit
#define KERNEL_ENOTTY perrtab_KERNEL->v[25 - ERRMIN_KERNEL].exit
#define KERNEL_ETXTBSY perrtab_KERNEL->v[26 - ERRMIN_KERNEL].exit
#define KERNEL_EFBIG perrtab_KERNEL->v[27 - ERRMIN_KERNEL].exit
#define KERNEL_ENOSPC perrtab_KERNEL->v[28 - ERRMIN_KERNEL].exit
#define KERNEL_ESPIPE perrtab_KERNEL->v[29 - ERRMIN_KERNEL].exit
#define KERNEL_EROFS perrtab_KERNEL->v[30 - ERRMIN_KERNEL].exit
#define KERNEL_EMLINK perrtab_KERNEL->v[31 - ERRMIN_KERNEL].exit
#define KERNEL_EPIPE perrtab_KERNEL->v[32 - ERRMIN_KERNEL].exit
#define KERNEL_EDOM perrtab_KERNEL->v[33 - ERRMIN_KERNEL].exit
#define KERNEL_ERANGE perrtab_KERNEL->v[34 - ERRMIN_KERNEL].exit
#define KERNEL_EDEADLK perrtab_KERNEL->v[35 - ERRMIN_KERNEL].exit
#define KERNEL_ENAMETOOLONG perrtab_KERNEL->v[36 - ERRMIN_KERNEL].exit
#define KERNEL_ENOLCK perrtab_KERNEL->v[37 - ERRMIN_KERNEL].exit
#define KERNEL_ENOSYS perrtab_KERNEL->v[38 - ERRMIN_KERNEL].exit
#define KERNEL_ENOTEMPTY perrtab_KERNEL->v[39 - ERRMIN_KERNEL].exit
#define KERNEL_ELOOP perrtab_KERNEL->v[40 - ERRMIN_KERNEL].exit
#define KERNEL_ENOMSG perrtab_KERNEL->v[42 - ERRMIN_KERNEL].exit
#define KERNEL_EIDRM perrtab_KERNEL->v[43 - ERRMIN_KERNEL].exit
#define KERNEL_ECHRNG perrtab_KERNEL->v[44 - ERRMIN_KERNEL].exit
#define KERNEL_ELNRNG perrtab_KERNEL->v[48 - ERRMIN_KERNEL].exit
#define KERNEL_EUNATCH perrtab_KERNEL->v[49 - ERRMIN_KERNEL].exit
#define KERNEL_ENOCSI perrtab_KERNEL->v[50 - ERRMIN_KERNEL].exit
#define KERNEL_EBADE perrtab_KERNEL->v[52 - ERRMIN_KERNEL].exit
#define KERNEL_EBADR perrtab_KERNEL->v[53 - ERRMIN_KERNEL].exit
#define KERNEL_EXFULL perrtab_KERNEL->v[54 - ERRMIN_KERNEL].exit
#define KERNEL_ENOANO perrtab_KERNEL->v[55 - ERRMIN_KERNEL].exit
#define KERNEL_EBADRQC perrtab_KERNEL->v[56 - ERRMIN_KERNEL].exit
#define KERNEL_EBADSLT perrtab_KERNEL->v[57 - ERRMIN_KERNEL].exit
#define KERNEL_EBFONT perrtab_KERNEL->v[59 - ERRMIN_KERNEL].exit
#define KERNEL_ENOSTR perrtab_KERNEL->v[60 - ERRMIN_KERNEL].exit
#define KERNEL_ENODATA perrtab_KERNEL->v[61 - ERRMIN_KERNEL].exit
#define KERNEL_ETIME perrtab_KERNEL->v[62 - ERRMIN_KERNEL].exit
#define KERNEL_ENOSR perrtab_KERNEL->v[63 - ERRMIN_KERNEL].exit
#define KERNEL_ENONET perrtab_KERNEL->v[64 - ERRMIN_KERNEL].exit
#define KERNEL_ENOPKG perrtab_KERNEL->v[65 - ERRMIN_KERNEL].exit
#define KERNEL_EREMOTE perrtab_KERNEL->v[66 - ERRMIN_KERNEL].exit
#define KERNEL_ENOLINK perrtab_KERNEL->v[67 - ERRMIN_KERNEL].exit
#define KERNEL_EADV perrtab_KERNEL->v[68 - ERRMIN_KERNEL].exit
#define KERNEL_ESRMNT perrtab_KERNEL->v[69 - ERRMIN_KERNEL].exit
#define KERNEL_ECOMM perrtab_KERNEL->v[70 - ERRMIN_KERNEL].exit
#define KERNEL_EPROTO perrtab_KERNEL->v[71 - ERRMIN_KERNEL].exit
#define KERNEL_EMULTIHOP perrtab_KERNEL->v[72 - ERRMIN_KERNEL].exit
#define KERNEL_EDOTDOT perrtab_KERNEL->v[73 - ERRMIN_KERNEL].exit
#define KERNEL_EBADMSG perrtab_KERNEL->v[74 - ERRMIN_KERNEL].exit
#define KERNEL_EOVERFLOW perrtab_KERNEL->v[75 - ERRMIN_KERNEL].exit
#define KERNEL_ENOTUNIQ perrtab_KERNEL->v[76 - ERRMIN_KERNEL].exit
#define KERNEL_EBADFD perrtab_KERNEL->v[77 - ERRMIN_KERNEL].exit
#define KERNEL_EREMCHG perrtab_KERNEL->v[78 - ERRMIN_KERNEL].exit
#define KERNEL_ELIBACC perrtab_KERNEL->v[79 - ERRMIN_KERNEL].exit
#define KERNEL_ELIBBAD perrtab_KERNEL->v[80 - ERRMIN_KERNEL].exit
#define KERNEL_ELIBSCN perrtab_KERNEL->v[81 - ERRMIN_KERNEL].exit
#define KERNEL_ELIBMAX perrtab_KERNEL->v[82 - ERRMIN_KERNEL].exit
#define KERNEL_ELIBEXEC perrtab_KERNEL->v[83 - ERRMIN_KERNEL].exit
#define KERNEL_EILSEQ perrtab_KERNEL->v[84 - ERRMIN_KERNEL].exit
#define KERNEL_ERESTART perrtab_KERNEL->v[85 - ERRMIN_KERNEL].exit
#define KERNEL_ESTRPIPE perrtab_KERNEL->v[86 - ERRMIN_KERNEL].exit
#define KERNEL_EUSERS perrtab_KERNEL->v[87 - ERRMIN_KERNEL].exit
#define KERNEL_ENOTSOCK perrtab_KERNEL->v[88 - ERRMIN_KERNEL].exit
#define KERNEL_EDESTADDRREQ perrtab_KERNEL->v[89 - ERRMIN_KERNEL].exit
#define KERNEL_EMSGSIZE perrtab_KERNEL->v[90 - ERRMIN_KERNEL].exit
#define KERNEL_EPROTOTYPE perrtab_KERNEL->v[91 - ERRMIN_KERNEL].exit
#define KERNEL_ENOPROTOOPT perrtab_KERNEL->v[92 - ERRMIN_KERNEL].exit
#define KERNEL_EPROTONOSUPPORT perrtab_KERNEL->v[93 - ERRMIN_KERNEL].exit
#define KERNEL_ESOCKTNOSUPPORT perrtab_KERNEL->v[94 - ERRMIN_KERNEL].exit
#define KERNEL_EOPNOTSUPP perrtab_KERNEL->v[95 - ERRMIN_KERNEL].exit
#define KERNEL_EPFNOSUPPORT perrtab_KERNEL->v[96 - ERRMIN_KERNEL].exit
#define KERNEL_EAFNOSUPPORT perrtab_KERNEL->v[97 - ERRMIN_KERNEL].exit
#define KERNEL_EADDRINUSE perrtab_KERNEL->v[98 - ERRMIN_KERNEL].exit
#define KERNEL_EADDRNOTAVAIL perrtab_KERNEL->v[99 - ERRMIN_KERNEL].exit
#define KERNEL_ENETDOWN perrtab_KERNEL->v[100 - ERRMIN_KERNEL].exit
#define KERNEL_ENETUNREACH perrtab_KERNEL->v[101 - ERRMIN_KERNEL].exit
#define KERNEL_ENETRESET perrtab_KERNEL->v[102 - ERRMIN_KERNEL].exit
#define KERNEL_ECONNABORTED perrtab_KERNEL->v[103 - ERRMIN_KERNEL].exit
#define KERNEL_ECONNRESET perrtab_KERNEL->v[104 - ERRMIN_KERNEL].exit
#define KERNEL_ENOBUFS perrtab_KERNEL->v[105 - ERRMIN_KERNEL].exit
#define KERNEL_EISCONN perrtab_KERNEL->v[106 - ERRMIN_KERNEL].exit
#define KERNEL_ENOTCONN perrtab_KERNEL->v[107 - ERRMIN_KERNEL].exit
#define KERNEL_ESHUTDOWN perrtab_KERNEL->v[108 - ERRMIN_KERNEL].exit
#define KERNEL_ETOOMANYREFS perrtab_KERNEL->v[109 - ERRMIN_KERNEL].exit
#define KERNEL_ETIMEDOUT perrtab_KERNEL->v[110 - ERRMIN_KERNEL].exit
#define KERNEL_ECONNREFUSED perrtab_KERNEL->v[111 - ERRMIN_KERNEL].exit
#define KERNEL_EHOSTDOWN perrtab_KERNEL->v[112 - ERRMIN_KERNEL].exit
#define KERNEL_EHOSTUNREACH perrtab_KERNEL->v[113 - ERRMIN_KERNEL].exit
#define KERNEL_EALREADY perrtab_KERNEL->v[114 - ERRMIN_KERNEL].exit
#define KERNEL_EINPROGRESS perrtab_KERNEL->v[115 - ERRMIN_KERNEL].exit
#define KERNEL_ESTALE perrtab_KERNEL->v[116 - ERRMIN_KERNEL].exit
#define KERNEL_EUCLEAN perrtab_KERNEL->v[117 - ERRMIN_KERNEL].exit
#define KERNEL_ENOTNAM perrtab_KERNEL->v[118 - ERRMIN_KERNEL].exit
#define KERNEL_ENAVAIL perrtab_KERNEL->v[119 - ERRMIN_KERNEL].exit
#define KERNEL_EISNAM perrtab_KERNEL->v[120 - ERRMIN_KERNEL].exit
#define KERNEL_EREMOTEIO perrtab_KERNEL->v[121 - ERRMIN_KERNEL].exit
#define KERNEL_EDQUOT perrtab_KERNEL->v[122 - ERRMIN_KERNEL].exit
#define KERNEL_ENOMEDIUM perrtab_KERNEL->v[123 - ERRMIN_KERNEL].exit
#define KERNEL_EMEDIUMTYPE perrtab_KERNEL->v[124 - ERRMIN_KERNEL].exit
#define KERNEL_ECANCELED perrtab_KERNEL->v[125 - ERRMIN_KERNEL].exit
#define KERNEL_ENOKEY perrtab_KERNEL->v[126 - ERRMIN_KERNEL].exit
#define KERNEL_EKEYEXPIRED perrtab_KERNEL->v[127 - ERRMIN_KERNEL].exit
#define KERNEL_EKEYREVOKED perrtab_KERNEL->v[128 - ERRMIN_KERNEL].exit
#define KERNEL_EKEYREJECTED perrtab_KERNEL->v[129 - ERRMIN_KERNEL].exit
#define KERNEL_EOWNERDEAD perrtab_KERNEL->v[130 - ERRMIN_KERNEL].exit
#define KERNEL_ENOTRECOVERABLE perrtab_KERNEL->v[131 - ERRMIN_KERNEL].exit
#define KERNEL_ERFKILL perrtab_KERNEL->v[132 - ERRMIN_KERNEL].exit
#define KERNEL_EHWPOISON perrtab_KERNEL->v[133 - ERRMIN_KERNEL].exit
#else
enum {
#define _E(a, b, c) KERNEL_ ## b = a,
ETABLE_KERNEL
#undef _E
};
#endif
#define ERRMIN_KERNEL 1
#define ERRMAX_KERNEL 133
extern errtab * perrtab_KERNEL;
#endif
