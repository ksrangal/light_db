#include "wal.hxx"

namespace DB
{

std::string WalEntry::serialize() const
{
  std::string serialized_wal; 

  serialized_wal += std::to_string(static_cast<int>(wal_type)) + "\n";
  serialized_wal += std::to_string(timestamp) + "\n";
  serialized_wal += key + "\n";
  serialized_wal += value + "\n";

  return serialized_wal; 
}

WalEntry WalEntry::deserialize(std::istream& serialized_wal)
{
  std::string line;
  WalType wal_type; 
  long long timestamp;
  std::string key;  
  std::string value; 

  /*
   * Persistent store format :-  
 	0
	1758901528482
	k1
	v1
  */
  if ( !std::getline(serialized_wal, line) ) 
    throw std::runtime_error("Failed to get operation type from store");
  else 
    wal_type = static_cast<WalType>(std::stoi(line));
  
  if ( !std::getline(serialized_wal, line) ) 
    throw std::runtime_error("Failed to get timestamp from store");
  else 
    timestamp = std::stoll(line);
    
  if ( !std::getline(serialized_wal, line) ) 
    throw std::runtime_error("Failed to get key from store");
  else 
    key = line;

  if ( !std::getline(serialized_wal, line) ) 
    throw std::runtime_error("Failed to get key value from store");
  else 
    value = line;

  return WalEntry(key, value, wal_type, timestamp);
}

} // DB namespace 
