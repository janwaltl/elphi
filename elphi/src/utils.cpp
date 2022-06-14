#include <cstring>

#include <elphi/utils.hpp>

namespace elphi {
std::string
strerror(int err) {
    std::string buffer;
    buffer.resize(256);
    // g++ for some reason defines GNU_SOURCE -> GNU specific implementation
    // which return through retval is used.
    // Buffer might or might not be used.
    char* ret = strerror_r(err, buffer.data(), buffer.size());
    return std::string{ret};
}
} // namespace elphi
