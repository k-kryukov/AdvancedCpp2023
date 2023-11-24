#include <string>
#include <string_view>

#include <boost/uuid/uuid.hpp>

class Note {
    boost::uuids::uuid id_;
    std::string text_;
public:
    Note(std::string_view text);

    auto getID() const { return id_; }
    auto getText() const { return text_; }
};