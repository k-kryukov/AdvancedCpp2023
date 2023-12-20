#include <unordered_map>
#include <algorithm>
#include <glog/logging.h>
#include <memory>

#include "Note.hpp"

class UserData {
    // id -> Note
    std::vector<std::shared_ptr<Note>> notes_;
public:
    void addNote(std::shared_ptr<Note> note);

    void removeNote(uint64_t id) {
        if (id >= notes_.size()) {
            std::stringstream ss;
            ss << "Note does not exist; ID : " << id;

            throw std::invalid_argument{std::move(ss).str()};
        }

        DLOG(INFO) << "Prev notes cnt: " << notes_.size();
        notes_.erase(notes_.begin() + id);
        DLOG(INFO) << "Cur notes cnt: " << notes_.size();
    }

    std::vector<std::shared_ptr<Note>> getNotes() {
        DLOG(INFO) << "Dumping notes...";

        return notes_;
    }
};