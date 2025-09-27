#include <iostream>

namespace DB
{

/* Opertion type */
enum class WalType
{
  ADD,       // add a new key  
  UPDATE,    // update an existing key 
  DELETE     // delete an existing key
};

/* This class define the write ahead (WAL) entry in the persistent store. */
class WalEntry
{
  private: 
    /* Operation type on the key */
    WalType wal_type; 

    /* Key name */
    std::string key;
 
    /* Key value */
    std::string value; 

    /* Timstamp when key was stored in the persistent store. */ 
    long long timestamp; 
      
  public: 
    WalEntry(const std::string &k, 
             const std::string &v, 
             WalType           wt, 
             long long         ts) : 
      key(k), value(v), wal_type(wt), timestamp(ts)
    {
    } 

    /* Serialize this wal entry. */
    std::string serialize() const;
  
    /* Deserialize the wal entry from string*/
    static WalEntry deserialize(std::istream& serialized_wal); 

    /* Getters */
    const std::string getKey() const
    {
      return key; 
    }
 
    const std::string getValue() const
    {
      return value; 
    }

    WalType getWalType() const 
    {
      return wal_type;
    }
};

} // DB namespace
