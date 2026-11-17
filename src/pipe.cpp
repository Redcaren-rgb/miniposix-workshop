#include "miniposix/pipe.hpp"

#include <cerrno>
#include <system_error>
#include <unistd.h>

namespace miniposix {

Pipe Pipe::create() {
    int fds[2];

    if (::pipe(fds) == -1) {
        throw std::system_error(
            errno,
            std::generic_category(),
            "pipe creation failed"
        );
    }

    Pipe result;

    result.read_end_.reset(fds[0]);
    result.write_end_.reset(fds[1]);

    return result;
}

} // namespace miniposix

