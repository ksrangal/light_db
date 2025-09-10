#include "db.hxx"

int main()
{
  KV kv; 
  kv.set("k1", "v1");
  kv.print("k1"); // shd be v1-1

  kv.begin();
  kv.set("k1", "v1-2");
  kv.commit();
  kv.print("k1");  // shd be v1-2

  kv.begin();
  kv.set("k1", "v1-3");
  kv.rollback();
  kv.print("k1");  // shd be v1-2
  kv.commit();
  kv.print("k1");  // shd be v1-2 (txn rolled back) 

  // nested case - rollback inner txn
  kv.begin();
  kv.set("k1", "v1-3");
	  kv.begin();
	  kv.set("k1", "v1-4");
	  kv.rollback(); 
	  
  kv.print("k1");  // shd be v1-2
  kv.commit();
  kv.print("k1"); // shd be v1-3 (outter txn comitted)
 

  // nested case - rollback outer txn
  kv.begin();
  kv.set("k1", "v1-4");
	  kv.begin();
	  kv.set("k1", "v1-5");
	  kv.commit(); 
	  
  kv.print("k1");  // shd be v1-3 (from main store)
  kv.rollback();

  kv.commit();
  kv.print("k1"); // shd be v1-3 (top level txn was rolled back) 

  return 0;
}

