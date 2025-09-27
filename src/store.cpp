#include "store.hxx"
#include <chrono>
#include <string>

namespace DB
{

Store::Store(const std::string& path) : store_path(path)
{
  /* binary mode : A text stream is an ordered sequence
   * of characters that can be composed into lines; 
   * a line can be decomposed into zero or more characters 
   * plus a terminating '\n' (“newline”) character.*/
  in_store.open(store_path, std::ios::app | std::ios::binary); 
  if ( ! in_store.is_open() ) 
  {
    throw std::runtime_error("Failed to open" + store_path); 
  }
}

Store::~Store()
{
  if ( in_store.is_open() )       
    in_store.close();
}

void Store::appendEntry(WalType           wal_type, 
                        const std::string &key, 
                        const std::string &value) 
{
  auto now = std::chrono::system_clock::now(); 
  auto duration = now.time_since_epoch();
  auto microseconds = 
    std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

  /* Create WalEntry object */
  WalEntry entry = WalEntry(key, value, wal_type, microseconds);
  std::string serialized_entry = entry.serialize();

  /* Wrtie to the disk. */
  in_store << serialized_entry; 
  in_store.flush();
}

void 
Store::warmUp(std::unordered_map<std::string, std::string> &in_memory)
{
  out_store.open(store_path, std::ios::binary); 
  if ( !out_store.is_open() ) 
    throw std::runtime_error("Failed to open file: "+ store_path);

  while ( out_store.peek() != EOF)
  {
    try 
    {
      /* desearialize the log entry from the disk */
      auto wal_entry = WalEntry::deserialize(out_store); 
     
      // update the in memory cache. 
      if ( wal_entry.getWalType() == WalType::ADD ||
           wal_entry.getWalType() == WalType::UPDATE ) 
        in_memory[wal_entry.getKey()] = wal_entry.getValue();
      else 
        in_memory.erase(wal_entry.getKey());
    }
    catch (const std::exception &e)
    {
      std::cerr<< "Error occurred while reading from store: " << e.what() << std::endl; 
      break;
    }
  }
}

} // DB namespace
