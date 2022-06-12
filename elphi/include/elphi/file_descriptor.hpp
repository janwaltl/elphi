/*******************************************************************************
 * @file file_descriptor.hpp
 *
 * RAII wrapper around C file descriptor.
 ******************************************************************************/

#include <cstdint>

namespace elphi {
class FileDescriptor {
public:
    /*******************************************************************************
     * @brief Capture the given C descriptor.
     *
     * @param fd Descriptor to manage.
     ******************************************************************************/
    explicit FileDescriptor(int fd);

    /*******************************************************************************
     * @brief Move-only.
     ******************************************************************************/
    FileDescriptor(const FileDescriptor&) = delete;

    /*******************************************************************************
     * @brief Claim FD from @p other, leave it in closed state.
     ******************************************************************************/
    FileDescriptor(FileDescriptor&& other) noexcept;

    /*******************************************************************************
     * @brief Move-only.
     ******************************************************************************/
    FileDescriptor&
    operator=(const FileDescriptor&) = delete;

    /*******************************************************************************
     * @brief Close this FD, claim FD from @p other, leave it in closed state.
     ******************************************************************************/
    FileDescriptor&
    operator=(FileDescriptor&& other) noexcept;


    /*******************************************************************************
     * @brief Close descriptor on destruction.
     ******************************************************************************/
    virtual ~FileDescriptor();

    /*******************************************************************************
     * @brief Whether the descriptor is opened.
     ******************************************************************************/
    bool
    is_opened() const noexcept;

    /*******************************************************************************
     * @brief Close the file descriptor
     *
     * No-op for closed FDs.
     ******************************************************************************/
    void
    close() noexcept;

private:
    constexpr static int c_no_fd = -1;
    /*! Manage file descriptor. */
    int fd{c_no_fd};
};
} // namespace elphi
