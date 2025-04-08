#include <iostream>
#include <string>
#include <vector>
#include <sstream>  // for stringstream used in decompress function
using namespace std;
//AYBERK SAVAS 
// dictionary class for code and key implementation
class DictItem {
public:
    string key;
    int code;
    DictItem() : key(""), code(-1) {}
    DictItem(string k, int c) : key(k), code(c) {}

    bool operator==(const DictItem& other) const {
        return key == other.key;
    }

    bool operator!=(const DictItem& other) const {
        return !(*this == other);
    }

    friend unsigned int hash(const DictItem& item);
};
//i defined the hash function out of the hash class since you wanted hash class "to be general and  works for any arbitrary object class that overloads
//the operator == and/or !=" so in my implementation hash class is general it could be used for other objects after proper hash functions
//definitions(current one might not be suitable since it has a specific parameter called key) in conclusion hash class is general.

//hash function for dictItems
unsigned int hash(const DictItem& item) {
    int hashVal = 0;
    for (int i = 0; i < item.key.length(); ++i)
        hashVal = 37 * hashVal + item.key[i];
    return hashVal;
}



//hash table class that i took from course slides but changed into linear probing way
template <class HashedObj>
class HashTable {
public:
    explicit HashTable(const HashedObj& notFound, int size = 101);
    HashTable(const HashTable& rhs)
        : ITEM_NOT_FOUND(rhs.ITEM_NOT_FOUND), array(rhs.array), currentSize(rhs.currentSize) {}

    const HashedObj& find(const HashedObj& x) const;
    void insert(const HashedObj& x);
    void makeEmpty();
    void remove(const HashedObj& x);

    const HashTable& operator=(const HashTable& rhs);

    enum EntryType { ACTIVE, EMPTY, DELETED };

private:
    struct HashEntry {
        HashedObj element;
        EntryType info;

        HashEntry(const HashedObj& e = HashedObj(), EntryType i = EMPTY)
            : element(e), info(i) {}
    };

    bool isPrime(int n);
    int nextPrime(int n);
    void rehash();

    vector<HashEntry> array;
    int currentSize;
    const HashedObj ITEM_NOT_FOUND;


    bool isActive(int currentPos) const;
    int findPos(const HashedObj& x) const;

    unsigned int hash(const HashedObj &item) const {
        return ::hash(item);
    }

};

template <class HashedObj>
HashTable<HashedObj>::HashTable(const HashedObj& notFound, int size)
    : ITEM_NOT_FOUND(notFound), array(nextPrime(size)), currentSize(0) {
    makeEmpty();
}

template <class HashedObj>
int HashTable<HashedObj>::findPos(const HashedObj& x) const {
    int i = 0;   // number of collisions
    int p = hash(x) % array.size();  // current position

    while (array[p].info != EMPTY && array[p].element != x) {
        p += 1;  // add the difference
        if (p >= array.size())  // perform mod
            p -= array.size();  // if necessary
    }

    return p;
}

template <class HashedObj>
bool HashTable<HashedObj>::isActive(int currentPos) const {
    return array[currentPos].info == ACTIVE;
}

template <class HashedObj>
void HashTable<HashedObj>::remove(const HashedObj& x) {
    int currentPos = findPos(x);
    if (isActive(currentPos))
        array[currentPos].info = DELETED;
}

template <class HashedObj>
const HashedObj& HashTable<HashedObj>::find(const HashedObj& x) const {
    int currentPos = findPos(x);
    if (isActive(currentPos))
        return array[currentPos].element;

    return ITEM_NOT_FOUND;
}

template <class HashedObj>
void HashTable<HashedObj>::insert(const HashedObj& x) {
    int currentPos = findPos(x);
    if (isActive(currentPos))
        return;

    array[currentPos] = HashEntry(x, ACTIVE);
    // enlarge the hash table if necessary
    if (++currentSize > array.size() / 2)
        rehash();
}

template <class HashedObj>
void HashTable<HashedObj>::rehash() {
    vector<HashEntry> oldArray = array;
    // Create new double-sized, empty table
    array.resize(nextPrime(2 * array.size()));
    for (auto& entry : array)
        entry.info = EMPTY;
    // Copy table over
    currentSize = 0;
    for (const auto& entry : oldArray) {
        if (entry.info == ACTIVE)
            insert(entry.element);
    }
}

template <class HashedObj>
void HashTable<HashedObj>::makeEmpty() {
    for (auto& entry : array)
        entry.info = EMPTY;
    currentSize = 0;
}

template <class HashedObj>
int HashTable<HashedObj>::nextPrime(int n) {
    if (n % 2 == 0)
        n++;
    for (; !isPrime(n); n += 2)
        ;
    return n;
}

template <class HashedObj>
bool HashTable<HashedObj>::isPrime(int n) {
    if (n == 2 || n == 3)
        return true;
    if (n == 1 || n % 2 == 0)
        return false;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0)
            return false;
    return true;
}



//compressing function that works implements Ziv-Lemper Algorithm
string compress(string s, HashTable<DictItem>& t) {
    string result = "";
    int currentCode = 256;
    string currentPrefix = "";

    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
        string nextPrefix = currentPrefix + c;

        // check if the nextPrefix exists in the dictionary
        DictItem found = t.find(DictItem(nextPrefix, -1));
        if (found.code != -1) {  // if found in the dictionary, extend the prefix
            currentPrefix = nextPrefix;
        } else {
            // output the code for the current prefix
            result += to_string(t.find(DictItem(currentPrefix, -1)).code) + " ";
            if (currentCode < 4096) {
                // add the new string to the dictionary
                t.insert(DictItem(nextPrefix, currentCode++));
            }
            currentPrefix = string(1, c);  // start a new prefix with the current character
        }
    }

    // output the last code
    if (!currentPrefix.empty()) {
        result += to_string(t.find(DictItem(currentPrefix, -1)).code) + " ";
    }

    return result;
}

// decompressing according to the codes on by one
string decompress(string s, HashTable<DictItem>& t) {
    string result = "";
    //a vector of length 4096 since it is the max size of our dictionary
    vector<string> dict(4096);
    //initializing dictionary with ASCII chars(0 to 256)
    for (int i = 0; i < 256; ++i) {
        dict[i] = string(1, char(i));
    }

    stringstream ss(s); // to process the input string code by code
    int code;
    int previousCode = -1;
    int nextCode = 256; //first code will be assigned after ASCII range

    while (ss >> code) {
        string currentString;
        //CASE 1: code being smaller then 256(trivial ASCII table case) or already being in dictionary
        if (code < 256 || !dict[code].empty()) {
            currentString = dict[code];
        }
        //CASE 2: special case where the current code is equal to the nextCode
        else if (code == nextCode && !dict[previousCode].empty()) {
            currentString = dict[previousCode] + dict[previousCode][0];
        }
        else {
            currentString = "";
        }

        result += currentString;
        //adding a new entry to the dictionary if we had a valid previousCode
        if (previousCode != -1 && nextCode < 4096 && !currentString.empty()) {
            dict[nextCode++] = dict[previousCode] + currentString[0];
        }
        //move forward in the sequence
        previousCode = code;
    }

    return result;
}



int main() {
    //filling the first 256 slots of dictionary with ascii table chars
    HashTable<DictItem> t(DictItem("", -1), 4096);
    string current;
    int counter;
    for (counter = 0; counter < 256; ++counter) {
        current = char(counter);
        t.insert(DictItem(current, counter));
    }
    //user interface parts
    string input;
    char choice;
    cout << "To compress a file, press 1. To decompress a file, press 2: ";
    cin >> choice;

    if (choice == '1') {
        cout << "Enter the input string: ";
        cin.ignore();
        getline(cin,input);
        cout << "Compressed output: " << compress(input, t) << endl;
    } else if (choice == '2') {
        cout << "Enter the compressed string: ";
        cin.ignore();
        getline(cin,input);
        cout << "Decompressed string: " << decompress(input, t) << endl;
    }

    return 0;
}
