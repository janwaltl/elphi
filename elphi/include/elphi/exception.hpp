#pragma once

#include <stdexcept>

namespace elphi {
/*******************************************************************************
 * @brief Base exception class for ElPhi project.
 *
 * Inherits logic error because it has std::string ctor.
 ******************************************************************************/
class ElphiException : public std::logic_error {
public:
    using std::logic_error::logic_error;

    /*******************************************************************************
     * @brief Default copy ctor.
     ******************************************************************************/
    ElphiException(const ElphiException& other) = default;

    /*******************************************************************************
     * @brief Default move ctor.
     ******************************************************************************/
    ElphiException(ElphiException&& other) noexcept = default;

    /*******************************************************************************
     * @brief Default copy assignment.
     ******************************************************************************/
    ElphiException&
    operator=(const ElphiException& other) = default;

    /*******************************************************************************
     * @brief Default move assignment.
     ******************************************************************************/
    ElphiException&
    operator=(ElphiException&& other) noexcept = default;

    /*******************************************************************************
     * @brief Default destructor.
     ******************************************************************************/
    ~ElphiException() override = default;
};
} // namespace elphi
