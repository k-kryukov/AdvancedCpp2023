#include <system_error>

enum class ServiceError {
    USR_EXIST
};

class ServiceErrorCategory : public std::error_category {
    ServiceError err;
public:
    virtual const char* name() const noexcept {
        return "ServiceError";
    }
};