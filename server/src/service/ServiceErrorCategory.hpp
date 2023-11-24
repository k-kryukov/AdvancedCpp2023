// #include <system_error>

// enum class ServiceError {
//     USR_EXIST,
//     USR_NOT_EXIST
// };

// class ServiceErrorCategory : public std::error_category {
//     ServiceError err;
// public:
//     const char* name() const noexcept override {
//         return "ServiceError";
//     }

//     std::string message(int value) const override {
//         return std::generic_category().message(value);
//     }
// };