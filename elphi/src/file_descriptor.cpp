#include <utility>

#include <unistd.h>

#include <elphi/file_descriptor.hpp>

namespace elphi {

FileDescriptor::FileDescriptor(int fd) : fd(fd) {}

FileDescriptor::FileDescriptor(FileDescriptor&& other) noexcept : fd{std::exchange(other.fd, c_no_fd)} {}

FileDescriptor&
FileDescriptor::operator=(FileDescriptor&& other) noexcept {
    if (&other != this) {
        this->close();
        this->fd = std::exchange(other.fd, c_no_fd);
    }
    return *this;
}

FileDescriptor::~FileDescriptor() { this->close(); }

bool
FileDescriptor::is_opened() const noexcept {
    return fd != c_no_fd;
}

void
FileDescriptor::close() noexcept {
    if (fd != c_no_fd) {
        (void)::close(fd);
        fd = c_no_fd;
    }
}
} // namespace elphi
