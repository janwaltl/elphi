// Needed for XSI-compliant strerror_r.
#define _POSIX_C_SOURCE 200112L

#include <cstring>

#include <elphi/utils.hpp>


namespace elphi {
std::string
strerror(int err) {
    std::string buffer;
    buffer.resize(256);
    strerror_r(err, buffer.data(), buffer.size());
    buffer.resize(std::strlen(buffer.data()));
    return buffer;
}
} // namespace elphi
