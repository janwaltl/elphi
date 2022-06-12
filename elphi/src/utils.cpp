#include <cstring>

#include <elphi/utils.hpp>


namespace elphi {
std::string
strerror(int err) {
    std::string buffer;
    buffer.resize(256);
    char* ret = strerror_r(err, buffer.data(), buffer.size());
    (void)ret;
    buffer.resize(std::strlen(buffer.data()));
    return buffer;
}
} // namespace elphi
