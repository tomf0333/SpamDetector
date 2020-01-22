#include <iostream>
#include <vector>

#ifndef SPAMDETECTOR_HASHMAP_HPP
#define SPAMDETECTOR_HASHMAP_HPP

#define DEF_LOW_BOUND 0.25
#define DEF_HIGH_BOUND 0.75
#define DEF_CAPACITY 16
#define UP 1
#define DOWN 0

/**
 * a class that represents a generic hash map with 'keyT' as keys and 'valueT' as values.
 * saves for each map it's size (actual number of pairs in it), capacity (how much pairs you can put in it) and an
 * array of vectors of pairs that actually stores the pairs.
 * @tparam keyT- generic key type.
 * @tparam valueT- generic value type.
 */
template<typename keyT, typename valueT>
class HashMap
{
private:
    std::vector<std::pair<keyT, valueT>> *_buckets;
    int _size;
    int _capacity;

    /**
     * the hash function that clamps each key to it's number.
     * @param item- the received item (key).
     * @return- an int which is the hash of the given item.
     */
    int hashy(const keyT &item) const
    {
        return std::hash<keyT>()(item) & (_capacity - 1);
    }

    /**
     * function that finds the first index of the bucket that has any value, or returns the capacity
     * (not possible number) to indicate that no item was found.
     * *this was made to make something in the iterator implementation easier.
     * @return- the index of the first bucket that isn't empty.
     */
    int first() const
    {
        for (int i = 0; i < _capacity; i++)
        {
            if (!_buckets[i].empty())
            {
                return i;
            }
        }
        return _capacity;
    }

    /**
     * the function that resizes the map, receives the way to size (double by 2 or divide by 2).
     * it also takes care of rehashing.
     * @param way- an int to state if we need to double the map capacity or divide it.
     */
    void resize(int way)
    {
        int savedCapacity = _capacity;
        if (way == UP)
        {
            _capacity *= 2;
        }
        else
        {
            _capacity /= 2;
        }
        std::vector<std::pair<keyT, valueT>> *saved = _buckets;
        _size = 0;
        _buckets = new std::vector<std::pair<keyT, valueT>>[_capacity];
        clear();
        for (int i = 0; i < savedCapacity; i++)
        {
            for (auto &pair : saved[i])
            {
                insert(pair.first, pair.second);
            }
        }
        delete[] saved;
    }

public:
    /**
     * constructor for the hash map, initializes the bucket array and sets it's vectors and the size to 0.
     */
    HashMap() try : _buckets(new std::vector<std::pair<keyT, valueT>>[DEF_CAPACITY]), _size(0),
                    _capacity(DEF_CAPACITY)
    {
        for (int i = 0; i < DEF_CAPACITY; i++)
        {
            _buckets[i] = std::vector<std::pair<keyT, valueT>>(0);
        }
    }
    catch (const std::exception &ex)
    {
        throw ex;
    }

    /**
     * a constructor that receives two vectors representing the keys and values each, to build the map with.
     * it is arranged as : values[i] is the value of key[i], if duplicates are found the constructor
     * runs over the old value with the new one.
     * @param keys- the keys vector.
     * @param values- the values vector.
     */
    HashMap(const std::vector<keyT> &keys, const std::vector<valueT> &values) try
            : _buckets(new std::vector<std::pair<keyT, valueT>>[DEF_CAPACITY]), _size(0), _capacity(DEF_CAPACITY)
    {
        if (keys.size() != values.size())
        {
            throw std::exception();
        }
        int index = 0;
        while (index != keys.size())
        {
            (*this)[keys[index]] = values[index];
            index++;
        }
    }
    catch (const std::exception &ex)
    {
        throw ex;
    }

    /**
     * copy constructor, initilizes the members to the received hash map.
     * @param other- another hash map.
     */
    HashMap(const HashMap &other) try : _buckets(
            new std::vector<std::pair<keyT, valueT>>[other._capacity]), _size(other._size), _capacity(other._capacity)
    {
        for (int i = 0; i < other._capacity; i++)
        {
            _buckets[i] = std::vector<std::pair<keyT, valueT>>(0);
        }
        for (auto i = other.begin(); i != other.end(); i++)
        {
            insert((*i).first, (*i).second);
        }
    }
    catch (const std::exception &ex)
    {
        throw ex;
    }

    /**
     * destructor, destructs '_buckets' the buckets array.
     */
    ~HashMap()
    {
        delete[] _buckets;
    }

    /**
     * getter for the '_size' member.
     * @return this hash maps '_size'.
     */
    int size() const
    {
        return _size;
    }

    /**
    * getter for the '_capacity' member.
    * @return this hash maps '_capacity'.
    */
    int capacity() const
    {
        return _capacity;
    }

    /**
    * getter for the load factor of this hash map, calculated by '_size' / '_capacity'.
    * @return this hash maps load factor.
    */
    double getLoadFactor() const
    {
        return double(_size) / _capacity;
    }

    /**
     * function that states wether the whole map is empty.
     * @return- true if the map is empty and false otherwise.
     */
    bool empty()
    {
        return _size != 0;
    }

    /**
     * checks if the map contains a specific key.
     * @param key- the received key to check.
     * @return- true if the map has this key and false otherwise.
     */
    bool containsKey(const keyT &key) const
    {
        try
        {
            // if bucketIndex threw exception then the key isn't in the map
            bucketIndex(key);
            return true;
        }
        catch (std::exception &ex)
        {
            return false;
        }
    }

    /**
     * inserts a pair (received two fold) into the map.
     * resizes the map if there was a need to.
     * @param key- the key of the new pair.
     * @param value- the value of the new pair.
     * @return- true if the pair were added successfully and false otherwise.
     */
    bool insert(const keyT &key, const valueT &value)
    {
        if (containsKey(key))
        {
            return false;
        }
        else
        {
            int keyHash = hashy(key);
            _buckets[keyHash].push_back(std::pair<keyT, valueT>(key, value));
            _size++;
            if (getLoadFactor() > DEF_HIGH_BOUND)
            {
                resize(UP);
            }
            return true;
        }
    }

    /**
     * earase the pair whose key is received, from the map.
     * resizes the map if there was a need to.
     * @param key- the received key.
     * @return- true if the pair were removed successfully and false otherwise.
     */
    bool erase(const keyT &key)
    {
        try
        {
            // if bucketIndex threw exception then the key isn't in the map
            int i = bucketIndex(key);
            for (int j = 0; j < _buckets[i].size(); j++)
            {
                if (_buckets[i][j].first == key)
                {
                    auto pos = _buckets[i].begin();
                    pos += j;
                    _buckets[i].erase(pos);
                    _size--;
                    if (getLoadFactor() < DEF_LOW_BOUND)
                    {
                        resize(DOWN);
                    }
                    return true;
                }
            }
        }
        catch (std::exception &ex)
        {
            return false;
        }
    }

    /**
     * returns the value of the received key if it is in the map, throws an exception if it isn't.
     * @param key- the received key.
     * @return- the value of the given key if it is in the map, if it isn't no return value will be received
     * and the function would throw an error outside.
     */
    valueT at(const keyT &key) const
    {
        try
        {
            // if bucketIndex threw exception then the key isn't in the map
            int i = bucketIndex(key);
            for (int j = 0; j < _buckets[i].size(); j++)
            {
                if (_buckets[i][j].first == key)
                {
                    return _buckets[i][j].second;
                }
            }
        }
        catch (const std::exception &ex)
        {
            throw ex;
        }
    }

    /**
     * returns the size of the bucket that the received key is in.
     * @param key- the received key.
     * @return- the size of the bucket of the given key if it is in the map, if it isn't no return value will be
     * received and the function would throw an error outside.
     */
    int bucketSize(const keyT &key) const
    {
        try
        {
            int i = bucketIndex(key);
            return _buckets[i].size();
        }
        catch (const std::exception &ex)
        {
            throw ex;
        }
    }

    /**
     * returns the index of the bucket that the received key is in.
     * @param key- the received key.
     * @return- the index of the bucket of the given key if it is in the map, if it isn't no return value will be
     * received and the function would throw an error outside.
     */
    int bucketIndex(const keyT &key) const
    {
        int i = hashy(key);
        for (int j = 0; j < _buckets[i].size(); j++)
        {
            if (_buckets[i][j].first == key)
            {
                return i;
            }
        }
        throw std::exception();
    }

    /**
     * clears all the pairs from the map.
     */
    void clear()
    {
        for (int i = 0; i < _capacity; i++)
        {
            _buckets[i].clear();
        }
    }

    /**
     * a const forward iterator that runs on HashMap.
     * saves the map as composition and runs on the iterators of its's inner vectors.
     * also has the index of the bucket that it is working on.
     */
    class iterator
    {
    private:
        const HashMap *_hm;
        typename std::vector<std::pair<keyT, valueT>>::iterator _it;
        int _index;
    public:
        /**
         * constructor for the iterator, receives the map it will be linked to, an iterator of ont of it's inner
         * vectors, and the bucket index that that vector is.
         * @param hm- the HashMap object this iterator belongs to.
         * @param it- the iterator of the vector we are currently on.
         * @param bucketIndex- the index of the vector from last line.
         */
        iterator(const HashMap *hm, typename std::vector<std::pair<keyT, valueT>>::iterator it, int bucketIndex) : _hm(
                hm), _it(it), _index(bucketIndex)
        {
        }

        /**
         * prefix ++operator for the iterator, moves up the buckets until it found a valid pair then returns it.
         * @return- the next pair in the map working from bucket 0, to it's end() and to the next bucket and so on.
         */
        iterator operator++(int)
        {
            _it++;
            while (_it == _hm->_buckets[_index].end() and _index < (_hm->_capacity - 1))
            {
                _index++;
                _it = _hm->_buckets[_index].begin();
            }
            return iterator(_hm, _it, _index);
        }

        /**
         * ++operator for the iterator, moves up the buckets until it found a valid pair then returns it.
         * @return- the next pair in the map working from bucket 0, to it's end() and to the next bucket and so on.
         */
        iterator operator++()
        {
            auto save = _it;
            (*this)++;
            return iterator(_hm, save, _index);
        }

        /**
         * dereferance operator, gets the pair from the vector iterator.
         * @return- the pair of 'keyT', 'valueT' that the iterator is currently pointing to.
         */
        const std::pair<keyT, valueT> &operator*() const
        {
            return *_it;
        }

        /**
         * dereferance operator, gets the pair from the vector iterator.
         * @return- the pair of 'keyT', 'valueT' that the iterator is currently pointing to.
         */
        const std::pair<keyT, valueT> *operator->() const
        {
            return _it;
        }

        /**
         * ==operator, compares the current iterator with another one.
         * @param rhs- the other iterator.
         * @return- true if both iterators are the same and false otherwise.
         */
        bool operator==(const iterator &rhs) const
        {
            return (_it == rhs._it);
        }

        /**
         * !=operator, compares the current iterator with another one.
         * @param rhs- the other iterator.
         * @return- false if the iterators are different and true otherwise.
         */
        bool operator!=(const iterator &rhs) const
        {
            return (_it != rhs._it);
        }
    };

    /**
     * the end iterator of the map, points to the end of the last buckets vector.
     * @return- an iterator to one after the last items possible in the map.
     */
    iterator end() const
    {
        return iterator(this, _buckets[_capacity - 1].end(), _capacity - 1);
    }

    /**
     * the begin iterator of the map, points to the begin of the first buckets vector.
     * @return- an iterator to the first possible item in the map.
     */
    iterator begin() const
    {
        int i = first();
        if (i == _capacity)
        {
            return end();
        }
        else
        {
            return iterator(this, _buckets[i].begin(), i);
        }
    }

    /**
     * =operator puts the item pairs of the given HashMap in the currecnt HashMap.
     * @param other- the other HashMap.
     * @return- the current HashMap after we insert the items of the other into it.
     */
    HashMap &operator=(const HashMap &other)
    {
        if (this != &other)
        {
            delete[] _buckets;
            _size = 0;
            _buckets = new std::vector<std::pair<keyT, valueT>>[other._capacity];
            _capacity = other._capacity;
            for (int i = 0; i < _capacity; i++)
            {
                _buckets[i] = std::vector<std::pair<keyT, valueT>>(0);
            }
            for (auto i = other.begin(); i != other.end(); i++)
            {
                insert((*i).first, (*i).second);
            }
        }
        return *this;
    }

    /**
     * []operator, returns a reference of the value of the given key, does not throw error if the key does not exist.
     * @param key- the given key.
     * @return- a reference to the value of the given key if it exists in the map, and if it doesn't then
     * it creates a dummy pair (real key, garbage value) and returns a reference to that.
     */
    valueT &operator[](const keyT &key)
    {
        if (!containsKey(key))
        {
            valueT a;
            insert(key, a);
        }
        int h = hashy(key);
        for (int i = 0; i < _buckets[h].size(); i++)
        {
            if (_buckets[h][i].first == key)
            {
                return _buckets[h][i].second;
            }
        }
    }

    /**
     * []operator, returns a reference of the value of the given key, does not throw error if the key does not exist.
     * @param key- the given key.
     * @return- a reference to the value of the given key if it exists in the map, and if it doesn't then
     * does nothing, undefined behavior.
     */
    valueT &operator[](const keyT &key) const
    {
        if (containsKey(key))
        {
            int h = hashy(key);
            for (int i = 0; i < _buckets[h].size(); i++)
            {
                if (_buckets[h][i].first == key)
                {
                    return _buckets[h][i].second;
                }
            }
        }
    }

    /**
    * ==operator for the HashMap, compares between the two HashMaps: size, keys and finally values.
    * @param other- the other HashMap to compare with.
    * @return- true if both HashMaps are equal in keys and values (does not check locations), false otherwise.
    */
    bool operator==(const HashMap &other) const
    {
        if (_size != other._size)
        {
            return false;
        }
        for (auto i = other.begin(); i != other.end(); i++)
        {
            try
            {
                if (at((*i).first) != (*i).second)
                {
                    return false;
                }
            }
            catch (const std::exception &ex)
            {
                return false;
            }
        }
        return true;
    }

    /**
    * !=operator for the HashMap, compares between the two HashMaps: size, keys and finally values.
    * @param other- the other HashMap to compare with.
    * @return- true if the HashMaps are different in some key and value, false otherwise.
    */
    bool operator!=(const HashMap &other) const
    {
        return !((*this) == other);
    }

//    void printer() const
//    {
//        for (int i = 0; i < _capacity; i++)
//        {
//            for (int j = 0; j < _buckets[i].size(); j++)
//            {
//                std::cout << i << " : " << j << std::endl;
//                std::cout << _buckets[i][j].first << " : " << _buckets[i][j].second << std::endl;
//            }
//        }
//    }
};


#endif //SPAMDETECTOR_HASHMAP_HPP
