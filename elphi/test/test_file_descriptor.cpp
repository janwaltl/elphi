#include <memory>

#include <catch2/catch_all.hpp>
#include <elphi/file_descriptor.hpp>

#include "mock_syscalls.hpp"

SCENARIO("File descriptor operations") {
    GIVEN("Opened raw FD") {
        size_t close_called_n = 0;
        const int exp_fd = 67;

        SysMock::use_mocked_syscall("close");
        SysMock::set_close_clbk([&close_called_n](int fd) {
            CHECK(fd == exp_fd);
            ++close_called_n;
            return 0;
        });

        WHEN("Managed FD is destroyed") {
            (void)elphi::FileDescriptor{exp_fd};

            THEN("FD is closed") { REQUIRE(close_called_n == 1); }
        }
        WHEN("Managed FD is explicitly closed") {

            auto man_fd{std::make_unique<elphi::FileDescriptor>(exp_fd)};
            man_fd->close();
            THEN("FD is closed") {
                REQUIRE(close_called_n == 1);
                REQUIRE(!man_fd->is_opened());
            }
            THEN("No double close") {
                man_fd.reset();
                REQUIRE(close_called_n == 1);
            }
        }

        WHEN("Managed FD constructed") {
            elphi::FileDescriptor man_fd{exp_fd};

            THEN("FD is considered opened") {
                REQUIRE(man_fd.is_opened());
                REQUIRE(close_called_n == 0);
            }
        }
        WHEN("Managed FD is move constructed") {
            // Ptr to manage their lifetime
            auto old_fd{std::make_unique<elphi::FileDescriptor>(exp_fd)};
            auto new_fd{std::make_unique<elphi::FileDescriptor>(std::move(*old_fd))};

            THEN("The underlying raw FD is moved correctly.") {
                REQUIRE(!old_fd->is_opened());
                REQUIRE(new_fd->is_opened());
                REQUIRE(close_called_n == 0);
            }
            THEN("Closing both -> no double close") {
                old_fd->close();
                new_fd->close();
                REQUIRE(close_called_n == 1);
            }
        }
        WHEN("Managed FD is reassigned") {
            // Ptr to manage their lifetime
            elphi::FileDescriptor old_fd{exp_fd};
            old_fd = elphi::FileDescriptor{-1};

            THEN("The underlying raw FD is closed before assignment") {
                REQUIRE(!old_fd.is_opened());
                REQUIRE(close_called_n == 1);
            }
        }
    }
}
