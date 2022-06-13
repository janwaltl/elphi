#include <utility>

#include <unistd.h>

#include <elphi/file_descriptor.hpp>

namespace elphi {

FileDescriptor::FileDescriptor(int fd) : m_fd(fd) {}

FileDescriptor::FileDescriptor(FileDescriptor&& other) noexcept : m_fd{std::exchange(other.m_fd, c_no_fd)} {}

FileDescriptor&
FileDescriptor::operator=(FileDescriptor&& other) noexcept {
    if (&other != this) {
        this->close();
        this->m_fd = std::exchange(other.m_fd, c_no_fd);
    }
    return *this;
}

FileDescriptor::~FileDescriptor() { this->close(); }

FileDescriptor::operator int() const noexcept { return raw(); }

int
FileDescriptor::raw() const noexcept {
    return m_fd;
}

bool
FileDescriptor::is_opened() const noexcept {
    return m_fd != c_no_fd;
}

void
FileDescriptor::close() noexcept {
    if (m_fd != c_no_fd) {
        (void)::close(m_fd);
        m_fd = c_no_fd;
    }
}
} // namespace elphi
