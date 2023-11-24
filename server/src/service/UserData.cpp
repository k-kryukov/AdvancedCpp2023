#include <memory>

#include "UserData.hpp"

uint64_t UserData::addNote(std::shared_ptr<Note> note) {
    notes_.emplace(nextID_, note);

    return nextID_++;
}