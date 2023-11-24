#include <unordered_map>
#include <algorithm>
#include <glog/logging.h>

#include "Note.hpp"

class UserData {
    // id -> Note
    std::unordered_map<size_t, std::shared_ptr<Note>> notes_;
    uint64_t nextID_{0};
public:
    uint64_t addNote(std::shared_ptr<Note> note);

    void removeNote(uint64_t id) {
        if (!notes_.contains(id)) {
            std::stringstream ss;
            ss << "Note does not exist; ID : " << id;

            throw std::invalid_argument{std::move(ss).str()};
        }

        notes_.erase(id);
    }

    std::vector<std::shared_ptr<Note>> getNotes() {
        DLOG(INFO) << "Dumping notes...";
        std::vector<std::shared_ptr<Note>> rv{notes_.size()};
        std::transform(notes_.begin(), notes_.end(), rv.begin(), [] (auto kv) { return kv.second; });

        return rv;
    }
};