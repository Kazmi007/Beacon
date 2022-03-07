#include "HashTable.h"

#include <cassert>
#include <iostream>
#include <iomanip>

const int KeyedHashTable::PRIME_LIST[PRIME_TABLE_COUNT] =
{
     2,    3,   5,   7,  11,  13,  17,  19,
     23,  29,  31,  37,  41,  43,  47,  53,
     59,  61,  67,  71,  73,  79,  83,  89,
     97, 101, 103, 107, 109, 113, 127, 131,
    137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223,
    227, 229, 233, 239, 241, 251, 257, 263,
    269, 271, 277, 281, 283, 293, 307, 311,
    313, 317, 331, 337, 347, 349, 353, 359,
    367, 373, 379, 383, 389, 397, 401, 409,
    419, 421, 431, 433, 439, 443, 449, 457,
    461, 463, 467, 479, 487, 491, 499, 503,
    509, 521, 523, 541
};

int KeyedHashTable::Hash(const std::string& key) const
{
    int hash = 0;
    for (int i=1; i<key.length(); i++) 
        hash += key[i] * PRIME_LIST[i];
    hash %= tableSize;
    return hash;
}

void KeyedHashTable::ReHash()
{
    KeyedHashTable nt(2 * tableSize);
    for (int i=0; i<tableSize; i++)
       if (table[i].key != "") 
            nt.Insert(table[i].key, table[i].intArray);
    delete[] table;
    tableSize = nt.tableSize;
    table = new HashData[tableSize];
    for (int i=0; i<tableSize; i++) 
        if (nt.table[i].key != "")
            table[i] = nt.table[i];
}

int KeyedHashTable::FindNearestLargerPrime(int requestedCapacity)
{
    for (int i=0; i<PRIME_TABLE_COUNT; i++)
        if (requestedCapacity < PRIME_LIST[i])
            return PRIME_LIST[i];
}

KeyedHashTable::KeyedHashTable()
{
    table = new HashData[2];
    tableSize = 2;
    occupiedElementCount = 0;
}

KeyedHashTable::KeyedHashTable(int requestedCapacity)
{
    int size = this->FindNearestLargerPrime(requestedCapacity);
    table = new HashData[size];
    tableSize = size;
    occupiedElementCount = 0;
}

KeyedHashTable::KeyedHashTable(const KeyedHashTable& other)
{
   table = new HashData[other.tableSize];
    for (int i=0; i<other.tableSize; i++) 
        table[i] = other.table[i];
    tableSize = other.tableSize;
    occupiedElementCount = other.occupiedElementCount;
}

KeyedHashTable& KeyedHashTable::operator=(const KeyedHashTable& other)
{
    delete[] table;
    table = new HashData[other.tableSize];
    for (int i=0; i<other.tableSize; i++) 
        table[i] = other.table[i];
    tableSize = other.tableSize;
    occupiedElementCount = other.occupiedElementCount;
}

KeyedHashTable::~KeyedHashTable()
{
    delete[] table;
}

bool KeyedHashTable::Insert(const std::string& key,
                            const std::vector<int>& intArray)
{
    int hash = this->Hash(key), i = 0;
    while (table[hash+(i*i)].key != "") {
        if (table[hash+(i*i)].key == key)
            return false;
        i++;
    }
    table[hash+(i*i)].intArray = intArray;
    table[hash+(i*i)].key = key;
    if ((occupiedElementCount * EXPAND_THRESHOLD) >= tableSize)
        this->ReHash();
    return true;
}

bool KeyedHashTable::Remove(const std::string& key)
{
    int hash = this->Hash(key), i = 0;
    while (table[hash+(i*i)].key != "") {
        if (table[hash+(i*i)].key == key) {
            table[hash+(i*i)].key = "";
            table[hash+(i*i)].intArray.clear();
            return true;
        }
        i++;
    }
    return false;
}

void KeyedHashTable::ClearTable()
{
   for (int i=0; i<tableSize; i++) {
       if (table[i].key != "") {
            table[i].key = "";
            table[i].intArray.clear();
       }
   }
   occupiedElementCount = 0;
}

bool KeyedHashTable::Find(std::vector<int>& valueOut,
                          const std::string& key) const
{
    int hash = this->Hash(key), i = 0;
    while (table[hash+(i*i)].key != "") {
        if (table[hash+(i*i)].key == key) {
            valueOut = table[hash+(i*i)].intArray;
            return true;
        }
        i++;
    }
    return false;
}

void KeyedHashTable::Print() const
{
    std::cout << "HT:";
    if(occupiedElementCount == 0)
    {
        std::cout << " Empty";
    }
    std::cout << "\n";
    for(int i = 0; i < tableSize; i++)
    {
        if(table[i].key == "") continue;

        std::cout << "[" << std::setw(3) << i << "] ";
        std::cout << "[" << table[i].key << "] ";
        std::cout << "[";
        for(size_t j = 0; j < table[i].intArray.size(); j++)
        {
            std::cout << table[i].intArray[j];
            if((j + 1) != table[i].intArray.size())
                std::cout << ", ";
        }
        std::cout << "]\n";
    }
    std::cout.flush();
}