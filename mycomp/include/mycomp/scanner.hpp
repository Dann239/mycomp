#pragma once

#include <string_view>

namespace mycomp {

struct Scanner {
    Scanner(std::string_view code) : code_(code) {}

    bool end() const {
        return ind_ == code_.size();
    }
    char curr() const {
        return end() ? '\n' : code_[ind_];
    }
    void advance(std::size_t delta = 1) {
        if(!end())
            ind_ += delta;
    }
    char peek(std::size_t delta = 1) const {
        return ind_ + delta < code_.size() ? code_[ind_ + delta] : '\n';
    }
    std::string_view substr(std::size_t delta = std::string_view::npos) {
        return code_.substr(ind_, delta);
    }
    std::size_t ind() const {
        return ind_;
    }
    std::string_view raw() const {
        return code_;
    }
private:
    std::string_view code_;
    std::size_t ind_ = 0;
};

}