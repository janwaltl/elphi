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
    if (SysMock::real_syscall["munmap"])
        return __real_munmap(addr, len);
    if (!SysMock::munmap_clbk) {
        FAIL_CHECK("When using mocked munmap, clbk must be provided.");
        return -1;
    }
    return SysMock::munmap_clbk(addr, len);
}

extern "C" int // NOLINTNEXTLINE
__real_close(int fd);

extern "C" int // NOLINTNEXTLINE
__wrap_close(int fd) {
    if (SysMock::real_syscall["close"])
        return __real_close(fd);
    if (!SysMock::close_clbk) {
        FAIL_CHECK("When using mocked close, clbk must be provided.");
        return -1;
    }
    return SysMock::close_clbk(fd);
}

extern "C" int // NOLINTNEXTLINE - unsigned long on purpose.
__real_open_perf_event(const perf_event_attr& attr, pid_t pid, int cpu, int group_fd, unsigned long flags);

extern "C" int // NOLINTNEXTLINE - unsigned long on purpose.
__wrap_open_perf_event(const perf_event_attr& attr, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    if (SysMock::real_syscall["perf_event"])
        return __real_open_perf_event(attr, pid, cpu, group_fd, flags);
    if (!SysMock::perf_event_clbk) {
        FAIL_CHECK("When using mocked perf_event_open, clbk must be provided.");
        return -1;
    }
    return SysMock::perf_event_clbk(attr, pid, cpu, group_fd, flags);
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

void
SysMock::set_close_clbk(CloseClbk clbk) {
    SysMock::use_mocked_syscall("close");
    SysMock::close_clbk = std::move(clbk);
}

void
SysMock::set_perf_event_clbk(PerfEventClbk clbk) {
    SysMock::use_mocked_syscall("perf_event");
    SysMock::perf_event_clbk = std::move(clbk);
}
