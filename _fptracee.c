#include "seccomp.h"

#include <linux/seccomp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <unistd.h>

static void usage() {
    fputs("Arguments: [-exclude-seccomp-unlink] [-exclude-seccomp-read-write] [-seccomp] -- program args...\n", stderr);
	exit(1);
}

int main(int argc, char **argv) {
    int sep;
    for (sep = 1; sep < argc && strcmp(argv[sep], "--") != 0; sep++);
    if (sep >= argc - 1)
        usage();
    bool withSeccomp = false;
    bool traceUnlink = true;
    bool traceReadWrite = true;
    for (int arg = 1; arg < sep; arg++) {
        if(strcmp(argv[arg], "-seccomp") == 0)
            withSeccomp = true;
        else if (strcmp(argv[arg], "-exclude-seccomp-unlink") == 0)
            traceUnlink = false;
        else if(strcmp(argv[arg], "-exclude-seccomp-read-write") == 0)
            traceReadWrite = false;
        else
            usage();
    }

    if (withSeccomp) {
        struct sock_fprog *seccomp_program = traceUnlink
            ? (traceReadWrite ? &seccomp_program_with_rw_with_unlink    : &seccomp_program_without_rw_with_unlink)
            : (traceReadWrite ? &seccomp_program_with_rw_without_unlink : &seccomp_program_without_rw_without_unlink);

        if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) {
            perror("no_new_privs failed");
        }
        if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, seccomp_program) < 0) {
            perror("seccomp failed");
        }
    }
    if (ptrace(PTRACE_TRACEME)) {
        perror("ptrace failed");
    }
    raise(SIGSTOP);
    execvp(argv[sep + 1], argv + sep + 1);
    fprintf(stderr, "execvp '%s'", argv[sep + 1]);
    perror(" failed");
}
