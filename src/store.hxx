#include "wal.hxx"
#include <fstream>

namespace DB
{

/* This class is responsible to store key/value on persistent store 
 * i.e file system. */
class Store
{
  private: 
    /* Persistent store path. */
    std::string store_path; 

    /* File stream to write to the store i.e file. */
    std::ofstream in_store; 

    /* File stream to read from the store i.e file. */
    std::ifstream out_store; 

    /* Append an entry to the persistent store. */
    void appendEntry(WalType           wal_type, 
                     const std::string &key, 
                     const std::string &value);

  public: 

    /* ctor, opens the store file handler for future appends */
    Store (const std::string& path);

    /* dtor, close the store file handler */ 
    ~Store();

    /* This method is used to add a new entry in the peristent store. */
    void add(const std::string &key, const std::string &value) 
    {
      appendEntry(WalType::ADD, key, value);
    }

    /* This method is used to update an entry in the peristent store. */
    void update(const std::string &key, const std::string &value) 
    {
      appendEntry(WalType::UPDATE, key, value);
    }

    /* This method is used to delete an entry from the peristent store. */
    void remove(const std::string &key, const std::string &value) 
    {
      appendEntry(WalType::DELETE, key, value);
    }

    /* Cache warm up. */
    void warmUp(std::unordered_map<std::string, std::string> &in_memory);
}; 

} // DB namespace
