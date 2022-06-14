/*******************************************************************************
 * Mocking framework for syscalls.
 ******************************************************************************/
#pragma once

#include <functional>
#include <map>
#include <string>
#include <string_view>

#include <sys/mman.h>

using MmapClbk = std::function<void*(void* addr, size_t len, int prot, int flags, int fd, off_t offset)>;
using MunmapClbk = std::function<int(void* addr, size_t len)>;
using CloseClbk = std::function<int(int fd)>;

/*******************************************************************************
 * @brief Singleton for mocking syscalls.
 *
 * Any method must be called within a test.
 ******************************************************************************/
class SysMock {
public:
    /*******************************************************************************
     * @brief Use mocked version of the specified syscall.
     *
     * @param syscall must be lowercase name of the syscall.
     ******************************************************************************/
    static void
    use_mocked_syscall(std::string_view syscall);

    /*******************************************************************************
     * @brief Use real version of the specified syscall.
     *
     * @param syscall must be lowercase name of the syscall.
     ******************************************************************************/
    static void
    use_real_syscall(std::string_view syscall);

    /*******************************************************************************
     * @brief Set mocked mmap() implementation.
     *
     * @param clbk Redirect mmap to this function.
     ******************************************************************************/
    static void
    set_mmap_clbk(MmapClbk clbk);

    /*******************************************************************************
     * @brief Set mocked munmap() implementation.
     *
     * @param clbk Redirect munmap to this function.
     ******************************************************************************/
    static void
    set_munmap_clbk(MunmapClbk clbk);

    /*******************************************************************************
     * @brief Set mocked close() implementation.
     *
     * @param clbk Redirect close to this function.
     ******************************************************************************/
    static void
    set_close_clbk(CloseClbk clbk);

    /*! Whether to use the real or mocked syscall. */
    inline static std::map<std::string, bool> real_syscall{{"mmap", true}, {"munmap", true}, {"close", true}};

    /*! Active mmap() mocked implementation. */
    inline static MmapClbk mmap_clbk{nullptr};
    /*! Active munmap() mocked implementation. */
    inline static MunmapClbk munmap_clbk{nullptr};
    /*! Active close() mocked implementation. */
    inline static CloseClbk close_clbk{nullptr};
};
