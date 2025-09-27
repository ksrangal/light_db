#include <iostream>
#include <unordered_map>
#include <stack>
#include <string>
#include <optional>
#include "store.hxx"

namespace DB 
{

class db 
{
private:

  /* In memory DB cache.  */
  std::unordered_map<std::string, std::string> cache; 

  /* Txn data, it is comitted into main DB store. */
  std::stack<std::unordered_map<std::string, std::optional<std::string>>> txn; 

  /* Persistent store. */
  Store store; 

public:

  db(std::string store_path) : store(Store(store_path))
  {
    /* build in memory cache from persistent store. */
    store.warmUp(cache);
  }

  /* Set/update key/valure in the DB store .*/
  void set(const std::string& key, const std::string& value);

  /* Get Key value. */ 
  std::optional<std::string> get(const std::string& key);

  /* Remove key/value from the store */
  void remove(std::string key); 

  /* txn begin */
  void begin();

  /* txn commit */
  void commit();

  /* txn rollback */
  void rollback();

  /* print key/value */
  void print(const std::string& key);
};

} // DB namespace 
