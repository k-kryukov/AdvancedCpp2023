#include <string>

using HashCls = std::hash<std::string>;

class Hasher {
    HashCls hash_;
public:
    uint64_t hash(std::string s) { return hash_(std::move(s)); }
};