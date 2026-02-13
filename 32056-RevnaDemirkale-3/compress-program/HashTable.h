#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <vector>
#include <functional>

/*
 * HashTable class
 * ----------------
 * A generic hash table implementation using open addressing
 * with linear probing for collision resolution.
 *
 * Key   : type of the key (must be hashable and comparable)
 * Value : type of the value
 */
template <class Key, class Value>
class HashTable {
public:
    /*
     * Constructor
     * ------------
     * Creates a hash table with the given size.
     * Default size is 8192.
     */
    HashTable(size_t tableSize = 8192) {
        table.resize(tableSize);
        makeEmpty();
    }

    /*
     * insert
     * ------
     * Inserts a key-value pair into the hash table.
     * Returns true if insertion is successful,
     * false if the key already exists.
     */
    bool insert(const Key &key, const Value &value) {
        size_t currentPos = findPos(key);

        // If the position is already occupied by an active entry,
        // insertion fails
        if (isActive(currentPos)) {
            return false;
        }

        // Insert the new key-value pair
        table[currentPos].key = key;
        table[currentPos].value = value;
        table[currentPos].info = ACTIVE;
        ++currentSize;

        return true;
    }

    /*
     * find
     * ----
     * Searches for the given key.
     * If found, stores the value in outValue and returns true.
     * Otherwise, returns false.
     */
    bool find(const Key &key, Value &outValue) const {
        size_t currentPos = findPos(key);

        // If the position is not active, the key does not exist
        if (!isActive(currentPos))
            return false;

        outValue = table[currentPos].value;
        return true;
    }

    /*
     * makeEmpty
     * ---------
     * Clears the hash table by marking all entries as EMPTY.
     */
    void makeEmpty() {
        currentSize = 0;
        for (size_t i = 0; i < table.size(); ++i)
            table[i].info = EMPTY;
    }

private:
    /*
     * EntryType
     * ---------
     * Represents the state of a hash table entry.
     */
    enum EntryType { ACTIVE, EMPTY, DELETED };

    /*
     * HashEntry
     * ---------
     * Represents a single slot in the hash table.
     */
    struct HashEntry {
        Key key;
        Value value;
        EntryType info;

        HashEntry(const Key &k = Key(),
                  const Value &v = Value(),
                  EntryType i = EMPTY)
            : key(k), value(v), info(i) { }
    };

    std::vector<HashEntry> table; // Hash table storage
    size_t currentSize;           // Number of active elements

    /*
     * isActive
     * --------
     * Returns true if the entry at currentPos is ACTIVE.
     */
    bool isActive(size_t currentPos) const {
        return table[currentPos].info == ACTIVE;
    }

    /*
     * findPos
     * -------
     * Finds the position of a key using linear probing.
     * If the key exists, returns its position.
     * Otherwise, returns the position where it can be inserted.
     */
    size_t findPos(const Key &key) const {
        size_t currentPos = myhash(key);
        size_t initialPos = currentPos;

        // Linear probing: move forward until an EMPTY slot
        // or the key is found
        while (table[currentPos].info != EMPTY &&
               !(table[currentPos].key == key)) {

            currentPos++;

            // Wrap around if needed
            if (currentPos >= table.size())
                currentPos = 0;

            // If we return to the initial position, stop
            if (currentPos == initialPos)
                break;
        }

        return currentPos;
    }

    /*
     * myhash
     * ------
     * Computes the hash value of the key
     * and maps it to the table size.
     */
    size_t myhash(const Key &key) const {
        static std::hash<Key> hf;
        return hf(key) % table.size();
    }
};

#endif

