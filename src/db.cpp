#include "db.hxx"
#include <optional>

namespace DB
{
  // set api
  void db::set(const std::string& key, const std::string& value)
  {
    //if no txn, update the cache and persistent store. 
    if (txn.empty()) 
    {
      // persist data before storing. 
      if ( cache.find(key) == cache.end() )
      {
        store.add(key, value); 
      }
      else 
      {
        store.update(key, value); 
      }

      // Update in memory
      cache[key] = value; 
    } 
    else 
    {
      txn.top()[key] = value;
    }
  }

  // get api
  std::optional<std::string> db::get(const std::string& key)
  {
    /* Find value in the cache. */
    /* optionally, we can allow to scan the current txns. */
    if (cache.count(key))
      return cache[key];
    else 
      return std::nullopt;
  }

  // remove api
  void db::remove(std::string key) 
  {
    // if no txn, delete from the cache and add entry to the store. */
    if (txn.empty()) 
    {
      if ( cache.find(key) != cache.end() ) 
      {
        // update persistent store 
        store.remove(key, cache[key]); 

        // update in memory
        cache.erase(key);
      }
    } 
    else 
    {
      txn.top()[key] = std::nullopt;
    }
  }

  // txn begin
  void db::begin()
  {
    txn.push({}); // start txn. 
  }


  // txn commit 
  void db::commit()
  {
    if (txn.empty())
    {
      std::cout<< "empty txn" << std::endl; 
      return;
    }
  
    auto deepestTxn = txn.top(); 
    txn.pop(); 

    if (txn.empty()) 
    {
      // final commit will merge into the main store. 
      for (auto& [key, value] : deepestTxn)
      {
        if (value.has_value())
        {
          // persist data before storing. 
          if ( cache.find(key) == cache.end() )
          {
            store.add(key, value.value()); 
          }
          else 
          {
            store.update(key, value.value()); 
          }

          // update the cache 
          cache[key] = value.value();
        }
        else 
        {
          // update persistent store 
          store.remove(key, cache[key]); 

          // delete the key from cache. 
          cache.erase(key);
        }
      }
    }
    else 
    {
      // merge into one level above to the txn
      for (auto& [key, value] : deepestTxn)
      {
        txn.top()[key] = value;
      }
   }
  }

  // txn rollback
  void db::rollback()
  {
    if (txn.empty())
    {
      std::cout<< "empty txn" << std::endl; 
      return;
    }
    
    // simply remove the entry from stack (txn).
    txn.pop(); 
  }

  void db::print(const std::string& key)
  {
    auto value = get(key); 
    if (value.has_value())
      std::cout << key<<"="<<value.value()<< std::endl;
    else
      std::cout << key<<"=<empty>"<< std::endl;
  }

} // DB namespace 
