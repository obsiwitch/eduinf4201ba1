#include <sys/stat.h>
#include <sys/sendfile.h>
#include <stdio.h>
#include "file_tools.h"

/**
 * Send a file (fd_src) to a destination corresponding to fd_dest.
 *
 * @param fd_src Source file descriptor
 * @param fd_dest Destination file descriptor
 * @return 0 on success, -1 otherwise.
 */
int fsendfile_helper(int fd_src, int fd_dest) {
    int status;
    struct stat st;

    status = fstat(fd_src, &st); // Retrieve file size
    if (status == -1) {
        perror("fstat");
        return -1;
    }

    status = sendfile(fd_dest, fd_src, NULL, st.st_size);
    if (status == -1) {
        perror("sendfile");
    }

    return status;
}
