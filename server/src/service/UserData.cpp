#include "UserData.hpp"

#include <memory>

void UserData::addNote(std::shared_ptr<Note> note) { notes_.push_back(note); }