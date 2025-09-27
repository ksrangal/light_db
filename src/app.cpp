#include "db.hxx"

using namespace DB; 
using namespace std;

void test_1() 
{
  db db("/tmp/db.wal"); 

  for (auto i=0; i<100; i++)
    db.set("k"+to_string(i), "v"+to_string(i)); 

  for (auto i=0; i<100; i++)
    db.print("k"+to_string(i)); 

  for (auto i=0; i<100; i++)
    db.set("k"+to_string(i), "n-v"+to_string(i)); 

  for (auto i=0; i<100; i++)
    db.print("k"+to_string(i)); 

#if 0
  for (auto i=0; i<100; i++)
    db.remove("k"+to_string(i)); 

  for (auto i=0; i<100; i++)
    db.print("k"+to_string(i)); 
#endif 

  return;
}

void test_2() 
{
  db db("/tmp/db.wal"); 

  for (auto i=0; i<100; i++)
    db.print("k"+to_string(i)); 

  return;
}

void test_3() 
{
  db db("/tmp/db.wal"); 

  db.set("k1", "v1");
  db.print("k1"); // shd be v1-1

  db.begin();
  db.set("k1", "v1-2");
  db.commit();
  db.print("k1");  // shd be v1-2

  db.begin();
  db.set("k1", "v1-3");
  db.rollback();
  db.print("k1");  // shd be v1-2
  db.commit();
  db.print("k1");  // shd be v1-2 (txn rolled back) 

  // nested case - rollback inner txn
  db.begin();
  db.set("k1", "v1-3");
	  db.begin();
	  db.set("k1", "v1-4");
	  db.rollback(); 
	  
  db.print("k1");  // shd be v1-2
  db.commit();
  db.print("k1"); // shd be v1-3 (outter txn comitted)
 

  // nested case - rollback outer txn
  db.begin();
  db.set("k1", "v1-4");
	  db.begin();
	  db.set("k1", "v1-5");
	  db.commit(); 
	  
  db.print("k1");  // shd be v1-3 (from main store)
  db.rollback();

  db.commit();
  db.print("k1"); // shd be v1-3 (top level txn was rolled back) 

  return;
}

int main()
{
  // test db with persistent store.
  test_1(); 

  // test cache warm up
  // run this test after test_1, DB should have cached keys.
  test_2(); 

   test db with recursive transactions on persistent store.
  test_3();
}
