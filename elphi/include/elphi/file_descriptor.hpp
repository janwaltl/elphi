/*******************************************************************************
 * @file file_descriptor.hpp
 *
 * RAII wrapper around C file descriptor.
 ******************************************************************************/
#pragma once

#include <cstdint>

namespace elphi {
class FileDescriptor {
public:
    /*******************************************************************************
     * @brief Construct closed descriptor.
     ******************************************************************************/
    FileDescriptor() noexcept = default;

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
     * @brief Return the raw file descriptor.
     ******************************************************************************/
    explicit operator int() const noexcept;

    /*******************************************************************************
     * @brief Return the raw file descriptor.
     ******************************************************************************/
    int
    raw() const noexcept;

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
    int m_fd{c_no_fd};
};
} // namespace elphi
