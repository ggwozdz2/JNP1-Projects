#ifndef KVFIFO_H
#define KVFIFO_H

#include <memory>
#include <map>
#include <list>
#include <iterator>

using namespace std;

template<typename K, typename V>
class kvfifo {

private:
    //types
    using KV = pair<K, V>;
    using KV_list = list<KV>;
    using KV_it = typename KV_list::iterator;
    using KV_it_list = list<KV_it>;
    using iterators_map = map<K, KV_it_list>;
    using shared_it_map = shared_ptr<iterators_map>;
    using shared_list = shared_ptr<KV_list>;

    // map storing iterators of all elements with the same certain key
    shared_it_map shr_key_iterators;

    // main list storing all elements in their order
    shared_list shr_elements;

    bool is_it_changed = false;

    void copy_everything() {
        try {
            if (shr_elements.use_count() > 1 || shr_key_iterators.use_count() > 1) {
                shared_list new_elements = make_shared<KV_list>();
                shared_it_map new_key_iterators = make_shared<iterators_map>();
                for (auto it = shr_elements->begin();
                     it != shr_elements->end(); ++it) {
                    new_elements.get()->push_back(*it);
                    auto new_it = new_elements.get()->end();
                    new_it--;
                    (*new_key_iterators)[it->first].push_back(new_it);
                }
                swap(shr_elements, new_elements);
                swap(shr_key_iterators, new_key_iterators);
                is_it_changed = false;
            }
        } catch (...) {
            throw;
        }
    }

public:

    kvfifo() : shr_key_iterators(make_shared<iterators_map>()),
               shr_elements(make_shared<KV_list>()) {};

    kvfifo(kvfifo const &that) {
        shr_key_iterators = that.shr_key_iterators;
        shr_elements = that.shr_elements;
        if (that.is_it_changed) {
            try {
                copy_everything();
            } catch (...) {
                throw;
            }
        }
    }

    kvfifo(kvfifo &&that) noexcept: shr_key_iterators(move(that.shr_key_iterators)),
                                    shr_elements(move(that.shr_elements)),
                                    is_it_changed(move(that.is_it_changed)) {
        that = kvfifo();
    };

    kvfifo &operator=(kvfifo other) {
        swap(shr_key_iterators, other.shr_key_iterators);
        swap(shr_elements, other.shr_elements);
        if (other.is_it_changed) {
            try {
                copy_everything();
            } catch (...) {
                throw;
            }
        }
        return *this;
    }

    void push(K const &k, V const &v) {
        //copy everything if needed
        try {
            copy_everything();
        } catch (...) {
            throw;
        }

        //add new element to the end of the list and to map
        KV new_element = make_pair(k, v);
        try {
            shr_elements->push_back(new_element);
        } catch (...) {
            throw;
        }
        auto current_iterator = shr_elements->end();
        --current_iterator;
        auto it = shr_key_iterators.get()->find(k);
        if (it == shr_key_iterators.get()->end()) {
            shr_key_iterators.get()->insert({k, KV_it_list()});
            it = shr_key_iterators.get()->find(k);
        }
        try {
            it->second.push_back(current_iterator);
        } catch (...) {
            // queue does not change
            shr_elements->pop_back();
        }
        is_it_changed = false;
    };

    void pop() {
        //check exceptions
        if (shr_elements.get()->size() == 0)
            throw std::invalid_argument("Empty fifo");

        //copy everything if needed
        try {
            copy_everything();
        } catch (...) {
            throw;
        }

        auto current_iterator = shr_elements->begin();
        K k = current_iterator->first;
        shr_elements->pop_front();
        shr_key_iterators.get()->at(k).pop_front();
        if (shr_key_iterators.get()->at(k).empty())
            shr_key_iterators.get()->erase(k);
        is_it_changed = false;
    };

    void pop(K const &k) {
        //check exceptions
        if (shr_elements->size() == 0)
            throw invalid_argument("Empty fifo");
        if (shr_key_iterators->find(k) == shr_key_iterators->end()
            || shr_key_iterators->at(k).size() == 0)
            throw invalid_argument("No such key");

        //copy everything if needed
        try {
            copy_everything();
        } catch (...) {
            throw;
        }

        auto current_iterator = shr_key_iterators->at(k).begin();
        shr_elements->erase(*current_iterator);
        shr_key_iterators->at(k).pop_front();
        if (shr_key_iterators->at(k).size() == 0)
            shr_key_iterators->erase(k);
        is_it_changed = false;
    };

    void move_to_back(K const &k) {
        if (shr_elements->size() == 0)
            throw invalid_argument("Empty fifo");
        if (shr_key_iterators->find(k) == shr_key_iterators->end()
            || shr_key_iterators->at(k).size() == 0)
            throw invalid_argument("No such key");

        //copy everything if needed
        try {
            copy_everything();
        } catch (...) {
            throw;
        }

        KV_it_list &this_k_iterators = shr_key_iterators.get()->at(k);
        KV_it_list new_k_iterators;
        int ile = 0;
        for (typename KV_it_list::iterator it = this_k_iterators.begin(); it != this_k_iterators.end();) {
            ile++;
            KV_it current_iterator = *it;
            KV current_element = *current_iterator;
            shr_elements->erase(current_iterator);
            shr_elements->push_back(current_element);
            auto new_iterator = shr_elements.get()->end();
            new_iterator--;
            it = this_k_iterators.erase(it);
            new_k_iterators.push_back(new_iterator);
        }
        this_k_iterators = new_k_iterators;
        is_it_changed = false;
    }

    std::pair<K const &, V &> front() {
        if (shr_elements.get()->size() == 0)
            throw invalid_argument("Empty fifo");
        else {
            try {
                copy_everything();
            } catch (...) {
                throw;
            }
            is_it_changed = true;
            auto current_iterator = shr_elements->begin();
            return {current_iterator->first, current_iterator->second};
        }
    }

    std::pair<K const &, V const &> front() const {
        if (shr_elements.get()->size() == 0)
            throw std::invalid_argument("Empty fifo");
        else {
            auto current_iterator = shr_elements->begin();
            return {current_iterator->first, current_iterator->second};
        }
    }

    std::pair<K const &, V &> back() {
        if (shr_elements.get()->size() == 0)
            throw std::invalid_argument("Empty fifo");
        else {
            try {
                copy_everything();
            } catch (...) {
                throw;
            }
            is_it_changed = true;
            auto current_iterator = shr_elements->end();
            current_iterator--;
            return {current_iterator->first, current_iterator->second};
        }
    }

    std::pair<K const &, V const &> back() const {
        if (shr_elements.get()->size() == 0)
            throw invalid_argument("Empty fifo");
        else {
            auto current_iterator = shr_elements->end();
            current_iterator--;
            return {current_iterator->first, current_iterator->second};
        }
    }

    std::pair<K const &, V &> first(K const &k) {
        if (shr_elements->size() == 0)
            throw invalid_argument("Empty fifo");
        auto current_iterator = shr_key_iterators.get()->find(k);
        if (current_iterator == shr_key_iterators->end()
            || shr_key_iterators->at(k).size() == 0)
            throw invalid_argument("No such key");
        else {
            try {
                copy_everything();
            } catch (...) {
                throw;
            }
            is_it_changed = true;
            return {k, shr_key_iterators.get()->at(k).front()->second};
        }
    }

    std::pair<K const &, V const &> first(K const &k) const {
        auto current_iterator = shr_key_iterators.get()->find(k);
        if (shr_elements->size() == 0)
            throw invalid_argument("Empty fifo");
        if (current_iterator == shr_key_iterators->end()
            || shr_key_iterators->at(k).size() == 0)
            throw invalid_argument("No such key");
        else {
            return {k, current_iterator->second.front()->second};
        }
    }

    std::pair<K const &, V &> last(K const &k) {
        auto current_iterator = shr_key_iterators->find(k);
        if (shr_elements->size() == 0)
            throw invalid_argument("Empty fifo");
        if (current_iterator == shr_key_iterators->end()
            || shr_key_iterators->at(k).size() == 0)
            throw invalid_argument("No such key");
        else {
            try {
                copy_everything();
            } catch (...) {
                throw;
            }
            is_it_changed = true;
            return {k, shr_key_iterators.get()->at(k).back()->second};
        }
    }

    std::pair<K const &, V const &> last(K const &k) const {
        auto current_iterator = shr_key_iterators->find(k);
        if (shr_elements->size() == 0)
            throw invalid_argument("Empty fifo");
        if (current_iterator == shr_key_iterators->end()
            || shr_key_iterators->at(k).size() == 0)
            throw invalid_argument("No such key");
        else {
            return {k, current_iterator->second.back()->second};
        }
    }

    size_t size() const noexcept {
        return shr_elements.get()->size();
    }

    bool empty() const noexcept {
        return size() == 0;
    }

    size_t count(K const &k) const {
        auto it = shr_key_iterators.get()->find(k);
        if (it == shr_key_iterators.get()->end())
            return 0;
        else
            return it->second.size();
    }

    void clear() {
        try {
            copy_everything();
            shr_elements->clear();
            shr_key_iterators->clear();
        } catch (...) {
            throw;
        }
    }

    class k_iterator {
    private:
        using k_iterator_t = typename iterators_map::iterator;
        k_iterator_t my_iterator;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = K;
        using pointer = K *;
        using reference = K &;

        k_iterator(k_iterator_t their_iterator) : my_iterator(their_iterator) {}

        k_iterator() = default;

        //dereference operator for iterator returns key
        const K &operator*() const { return my_iterator->first; }

        k_iterator &operator++() {
            my_iterator++;
            return *this;
        }

        k_iterator operator++(int) {
            k_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        k_iterator &operator--() {
            my_iterator--;
            return *this;
        }

        k_iterator operator--(int) {
            k_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const kvfifo<K, V>::k_iterator &that) const {
            return my_iterator == that.my_iterator;
        }

        bool operator!=(const kvfifo<K, V>::k_iterator &that) const {
            return !(*this == that);
        }
    };


    k_iterator k_begin() const noexcept {
        return k_iterator(shr_key_iterators->begin());
    }

    k_iterator k_end() const noexcept {
        return k_iterator(shr_key_iterators->end());
    }
};

#endif // KVFIFO_H
