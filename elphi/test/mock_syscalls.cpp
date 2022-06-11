/*******************************************************************************
 * Use linker --wrap cmd
 ******************************************************************************/
#include "mock_syscalls.hpp"

#include <atomic>
#include <cerrno>
#include <cstring>
#include <string>

#include <sys/mman.h>

#include <catch2/catch_test_macros.hpp>

namespace {
void
assert_lowercase(std::string_view str) {
    for (char c : str)
        REQUIRE(std::tolower(c) == c);
}
} // namespace

extern "C" void* // NOLINTNEXTLINE
__real_mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset);

extern "C" void* // NOLINTNEXTLINE
__wrap_mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset) {
    if (SysMock::real_syscall["mmap"])
        return __real_mmap(addr, len, prot, flags, fd, offset);
    if (!SysMock::mmap_clbk) {
        FAIL_CHECK("When using mocked mmap, clbk must be provided.");
        return nullptr;
    }
    return SysMock::mmap_clbk(addr, len, prot, flags, fd, offset);
}

extern "C" int // NOLINTNEXTLINE
__real_munmap(void* addr, size_t len);

extern "C" int // NOLINTNEXTLINE
__wrap_munmap(void* addr, size_t len) {
    if (SysMock::real_syscall["mmap"])
        return __real_munmap(addr, len);
    if (!SysMock::mmap_clbk) {
        FAIL_CHECK("When using mocked mmap, clbk must be provided.");
        return -1;
    }
    return SysMock::munmap_clbk(addr, len);
}

void
SysMock::use_mocked_syscall(std::string_view syscall) {
    assert_lowercase(syscall);

    SysMock::real_syscall[std::string{syscall}] = false;
}

void
SysMock::use_real_syscall(std::string_view syscall) {
    assert_lowercase(syscall);

    SysMock::real_syscall[std::string{syscall}] = true;
}

void
SysMock::set_mmap_clbk(MmapClbk clbk) {
    SysMock::use_mocked_syscall("mmap");
    SysMock::mmap_clbk = std::move(clbk);
}

void
SysMock::set_munmap_clbk(MunmapClbk clbk) {
    SysMock::use_mocked_syscall("munmap");
    SysMock::munmap_clbk = std::move(clbk);
}
