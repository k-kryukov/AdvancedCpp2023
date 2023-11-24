#include "Note.hpp"

Note::Note(std::string_view text) : text_{text} {
    id_ = boost::uuids::uuid{};
}