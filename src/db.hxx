#include <iostream>
#include <unordered_map>
#include <stack>
#include <string>
#include <optional>

class KV
{
private:

  // Main KV store. 
  std::unordered_map<std::string, std::string> kv; 

  // Txn data, it is comitted into main KV store. 
  std::stack<std::unordered_map<std::string, std::optional<std::string>>> txn; 

public:

  // set api
  void set(const std::string& key, const std::string& value)
  {
    //if no txn, update the main KV store. 
    if (txn.empty()) 
    {
      kv[key] = value; 
    } 
    else 
    {
      txn.top()[key] = value;
    }
  }

  // get api
  std::optional<std::string> get(const std::string& key)
  {
    // main store always check KV store. 
    // optionally, we can allow to scan txn and get value.
    if (kv.count(key))
      return kv[key];
    else 
      return std::nullopt;
  }

  // remove api
  void remove(std::string key) 
  {
    // if no txn, delete from the main KV store. 
    if (txn.empty()) 
    {
      kv.erase(key);
    } 
    else 
    {
      txn.top()[key] = std::nullopt;
    }
  }

  // txn begin
  void begin()
  {
    txn.push({}); // start txn. 
  }

  // txn commit 
  void commit()
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
      // final commit will merge into the main KV store. 
      for (auto& [key, value] : deepestTxn)
      {
        if (value.has_value())
        {
          // update the KV
          kv[key] = value.value();
        }
        else 
        {
          // delete the key from KV store. 
          kv.erase(key);
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
  void rollback()
  {
    if (txn.empty())
    {
      std::cout<< "empty txn" << std::endl; 
      return;
    }
    
    // simply remove the entry from stack (txn).
    txn.pop(); 
  }

  void print(const std::string& key)
  {
    auto value = get(key); 
    if (value.has_value())
      std::cout << key<<"="<<value.value()<< std::endl;
    else
      std::cout << key<<"=<empty>"<< std::endl;
  }
};
