#include <iostream>
#include <cassert>
#include <functional>
#include <sstream>
using namespace std;
 
 
class CHash
{
  public:
  	CHash(size_t size ) : m_Size(size)
    {
      m_Table = new TItem*[m_Size];
      for(size_t i = 0; i < m_Size; ++i){
      	m_Table[i] = nullptr;
      }
      m_FirstOrder = m_LastOrder = nullptr;
    }
 
    CHash(const CHash & src )
		: m_Table(nullptr), m_Size(src.m_Size), m_FirstOrder(nullptr), m_LastOrder(nullptr)	
		{
			m_Table = new TItem*[m_Size];
			for(size_t i = 0; i < m_Size; ++i){
				m_Table[i] = nullptr;
			}
			
			for(TItem* curr = src.m_FirstOrder; curr != nullptr; curr = curr->m_NextOrder){
				TItem* copy = new TItem(curr->m_Key, curr->m_Val);
				
				size_t idx = hashFn(copy->m_Key, m_Size);
				copy->m_NextHash = m_Table[idx];
				m_Table[idx] = copy;
				
				if(m_LastOrder == nullptr){
					m_LastOrder = m_FirstOrder = copy;
				}
				else{
					m_LastOrder->m_NextOrder = copy;
					copy->m_PrevOrder = m_LastOrder;
					m_LastOrder = copy;
				}
			}
    }
 
    ~CHash(void )
    {
      TItem * curr = m_FirstOrder;
      while(curr){
      	TItem * temp = curr;
      	curr = curr->m_NextOrder;
      	delete temp;
      }
      delete [] m_Table;
    }
 
    CHash & operator=(const CHash  & src )
    {
    	if(this == &src){
    		return *this;
    	}
    	
    	TItem* toDel = m_FirstOrder;
    	while(toDel){
    		TItem* temp = toDel;
    		toDel = toDel->m_NextOrder;
    		delete temp;
    	}
    	delete [] m_Table;
    	
    	m_Size = src.m_Size;
    	m_FirstOrder = nullptr;
    	m_LastOrder = nullptr;
    	m_Table = nullptr;
    	
    	m_Table = new TItem*[m_Size];
    	for(size_t i = 0; i < m_Size; ++i){
    		m_Table[i] = nullptr;
    	}
    	
    	for(TItem* curr = src.m_FirstOrder; curr != nullptr; curr = curr->m_NextOrder){
    		TItem* copy = new TItem(curr->m_Key, curr->m_Val);
    		
    		size_t idx = hashFn(copy->m_Key, m_Size);
    		copy->m_NextHash = m_Table[idx];
    		m_Table[idx] = copy;
    		
    		if(!m_LastOrder){
    			m_FirstOrder = m_LastOrder = copy;
    		}
    		else{
    			m_LastOrder->m_NextOrder = copy;
    			copy->m_PrevOrder = m_LastOrder;
    			m_LastOrder = copy;
    		}
    	}
    	
    	return *this;
    }
 
    bool Ins ( const string & key, const string & val )
    {
      size_t idx = hashFn(key, m_Size);
      // checkneme jestli stejny key neexistuje v danem bucketu
      TItem* keyCheck = m_Table[idx];
      for(; keyCheck != nullptr; keyCheck = keyCheck->m_NextHash){
      	if(keyCheck->m_Key == key){
      		return false;
      	}
      }
      
      TItem* newNode = new TItem(key, val);
      
      newNode->m_NextHash = m_Table[idx];
      m_Table[idx] = newNode;
      
      if(!m_LastOrder){
      	m_FirstOrder = m_LastOrder = newNode;
      }
      else{
      	m_LastOrder->m_NextOrder = newNode;
      	newNode->m_PrevOrder = m_LastOrder;
      	m_LastOrder = newNode;
      }
      return true;
    }
 
    bool Del ( const string & key )
    {
      size_t idx = hashFn(key, m_Size);
      TItem* curr = m_Table[idx];
      TItem* parent = nullptr;
      while(curr){
      	if(curr->m_Key == key){ // mazeme
      		if(!parent){
      			m_Table[idx] = curr->m_NextHash;
      		}
      		else{
      			parent->m_NextHash = curr->m_NextHash;
      		}
      		
      		if(!curr->m_PrevOrder){
      			m_FirstOrder = curr->m_NextOrder;
      		}
      		else{
      			curr->m_PrevOrder->m_NextOrder = curr->m_NextOrder;
      		}
      		
      		if(!curr->m_NextOrder){
      			m_LastOrder = curr->m_PrevOrder;
      		}
      		else{
      			curr->m_NextOrder->m_PrevOrder = curr->m_PrevOrder;
      		}
      		
      		delete curr;
      		return true;
      	}
      	parent = curr;
      	curr = curr->m_NextHash;
      }
      return false;
    }
 
    bool IsSet ( const string & key ) const
    {
      size_t idx = hashFn(key, m_Size);
      
      TItem* curr = m_Table[idx];
      for(; curr != nullptr; curr = curr->m_NextHash){
      	if(curr->m_Key == key) return true;
      }
      return false;
    }
 
		template<typename Func>
    void ForEach ( Func f ) const
    {
      TItem * curr = m_FirstOrder;
      for(; curr != nullptr; curr = curr->m_NextOrder){
      	f(curr->m_Val);
      }
    }
 
  private:
    struct TItem
    {
                             TItem                         ( const string & key,
                                                             const string & val )
        : m_Key ( key ),
          m_Val ( val ),
          m_NextHash ( NULL ),
          m_NextOrder ( NULL ),
          m_PrevOrder ( NULL )
      {
      }
 
      string                 m_Key;
      string                 m_Val;
      TItem                * m_NextHash;
      TItem                * m_NextOrder;
      TItem                * m_PrevOrder;
    };
 
    TItem                 ** m_Table;
    size_t                   m_Size;
    TItem                  * m_FirstOrder;
    TItem                  * m_LastOrder;
 
    friend int main ( void );
 
    size_t                   hashFn                        ( const string & x,
                                                             size_t         modulus ) const
    {
      size_t h = 5381;
      for ( string::size_type c : x )
        h += ( h << 5 ) + c;
      return h % modulus;
    }
};
 
 
int main ( void )
{
  CHash x ( 100 );
 
  assert ( sizeof ( CHash ) == sizeof ( CHash::TItem * ) * 3 + sizeof ( size_t ) );
  assert ( sizeof ( CHash::TItem ) == sizeof ( CHash::TItem * ) * 3 + sizeof ( string ) * 2 );
  assert ( x . Ins ( "peg", "foo" )
           && x . m_Table[1] -> m_Key == "peg"
           && x . m_Table[1] -> m_Val == "foo"
           && x . m_FirstOrder == x . m_Table[1]
           && x . m_FirstOrder -> m_NextOrder == NULL
           && x . m_LastOrder == x . m_Table[1]
           && x . m_LastOrder -> m_PrevOrder == NULL );
  assert ( x . Ins ( "bwc", "bar" )
           && x . m_Table[45] -> m_Key == "bwc"
           && x . m_Table[45] -> m_Val == "bar"
           && x . m_FirstOrder == x . m_Table[1] 
           && x . m_FirstOrder -> m_NextOrder == x . m_Table[45]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder == NULL
           && x . m_LastOrder == x . m_Table[45]
           && x . m_LastOrder -> m_PrevOrder == x . m_Table[1]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( x . Ins ( "jdr", "test" )
           && x . m_Table[45] -> m_Key == "jdr"
           && x . m_Table[45] -> m_Val == "test"
           && x . m_Table[45] -> m_NextHash -> m_Key == "bwc"
           && x . m_Table[45] -> m_NextHash -> m_Val == "bar"
           && x . m_FirstOrder == x . m_Table[1]
           && x . m_FirstOrder -> m_NextOrder == x . m_Table[45] -> m_NextHash
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder == x . m_Table[45]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL
           && x . m_LastOrder == x . m_Table[45]
           && x . m_LastOrder -> m_PrevOrder == x . m_Table[45] -> m_NextHash
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[1]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( x . Ins ( "peg", "something" ) == false );
  assert ( x . Ins ( "peg", "foo" ) == false );
  assert ( x . Ins ( "bwc", "test" ) == false );
  assert ( x . Ins ( "jdr", "mudr" ) == false );
  assert ( x . Ins ( "asa", "butterfield" )
           && x . m_Table[22] -> m_Key == "asa"             
           && x . m_Table[22] -> m_Val == "butterfield"
           && x . m_FirstOrder == x . m_Table[1]
           && x . m_FirstOrder -> m_NextOrder == x . m_Table[45] -> m_NextHash
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder == x . m_Table[45]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL
           && x . m_LastOrder == x . m_Table[22]
           && x . m_LastOrder -> m_PrevOrder == x . m_Table[45]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[45] -> m_NextHash
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[1]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( x . IsSet ( "peg" ) );
  assert ( x . IsSet ( "asa" ) );
  assert ( x . IsSet ( "bwc" ) );
  assert ( x . IsSet ( "jdr" ) );
  assert ( x . IsSet ( "whf" ) == false );
  assert ( x . IsSet ( "xns" ) == false );
  assert ( x . IsSet ( "abc" ) == false );
  assert ( x . IsSet ( "ooj" ) == false );
  ostringstream oss;
  x . ForEach ( [&oss] ( const string & val )
    {
      oss << val << endl;
    } );
  assert ( oss . str () == "foo\nbar\ntest\nbutterfield\n" );
  CHash n ( x );
  assert ( n . m_Table[1] -> m_Key == "peg"
           && n . m_Table[1] -> m_Val == "foo"
           && n . m_Table[45] -> m_Key == "jdr"
           && n . m_Table[45] -> m_Val == "test"
           && n . m_Table[45] -> m_NextHash -> m_Key == "bwc"
           && n . m_Table[45] -> m_NextHash -> m_Val == "bar"
           && n . m_Table[22] -> m_Key == "asa"             
           && n . m_Table[22] -> m_Val == "butterfield"
           && n . m_FirstOrder == n . m_Table[1]
           && n . m_FirstOrder -> m_NextOrder == n . m_Table[45] -> m_NextHash
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder == n . m_Table[45]
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == n . m_Table[22]
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL
           && n . m_LastOrder == n . m_Table[22]
           && n . m_LastOrder -> m_PrevOrder == n . m_Table[45]
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder == n . m_Table[45] -> m_NextHash
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == n . m_Table[1]
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( n . Del ( "jdr" )
           && n . m_Table[45] -> m_Key == "bwc"
           && n . m_Table[45] -> m_Val == "bar"
           && n . m_FirstOrder == n . m_Table[1]
           && n . m_FirstOrder -> m_NextOrder == n . m_Table[45]
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder == n . m_Table[22]
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL
           && n . m_LastOrder == n . m_Table[22]
           && n . m_LastOrder -> m_PrevOrder == n . m_Table[45]
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder == n . m_Table[1]
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( n . IsSet ( "jdr" ) == false );
  assert ( x . m_Table[1] -> m_Key == "peg"
           && x . m_Table[1] -> m_Val == "foo"
           && x . m_Table[45] -> m_Key == "jdr"
           && x . m_Table[45] -> m_Val == "test"
           && x . m_Table[45] -> m_NextHash -> m_Key == "bwc"
           && x . m_Table[45] -> m_NextHash -> m_Val == "bar"
           && x . m_Table[22] -> m_Key == "asa"             
           && x . m_Table[22] -> m_Val == "butterfield"
           && x . m_FirstOrder == x . m_Table[1]
           && x . m_FirstOrder -> m_NextOrder == x . m_Table[45] -> m_NextHash
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder == x . m_Table[45]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL
           && x . m_LastOrder == x . m_Table[22]
           && x . m_LastOrder -> m_PrevOrder == x . m_Table[45]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[45] -> m_NextHash
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[1]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( n . Del ( "404" ) == false );
  assert ( n . Del ( "system32" ) == false );
  assert ( n . Del ( "root" ) == false );
  n = n;
  assert ( n . Ins ( "xns", "test" )
           && n . m_Table[22] -> m_Key == "xns"
           && n . m_Table[22] -> m_Val == "test"
           && n . m_Table[22] -> m_NextHash -> m_Key == "asa"             
           && n . m_Table[22] -> m_NextHash -> m_Val == "butterfield"
           && n . m_FirstOrder == n . m_Table[1]
           && n . m_FirstOrder -> m_NextOrder == n . m_Table[45]
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder == n . m_Table[22] -> m_NextHash
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == n . m_Table[22]
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL 
           && n . m_LastOrder == n . m_Table[22]
           && n . m_LastOrder -> m_PrevOrder == n . m_Table[22] -> m_NextHash
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder == n . m_Table[45]
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == n . m_Table[1]
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  x = n;
  assert ( x . m_Table[22] -> m_Key == "xns"
           && x . m_Table[22] -> m_Val == "test"
           && x . m_Table[22] -> m_NextHash -> m_Key == "asa"             
           && x . m_Table[22] -> m_NextHash -> m_Val == "butterfield"
           && x . m_FirstOrder == x . m_Table[1]
           && x . m_FirstOrder -> m_NextOrder == x . m_Table[45]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22] -> m_NextHash
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL 
           && x . m_LastOrder == x . m_Table[22]
           && x . m_LastOrder -> m_PrevOrder == x . m_Table[22] -> m_NextHash
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[45]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[1]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( x . Ins ( "whf", "FIT" )
           && x . m_Table[22] -> m_Key == "whf"
           && x . m_Table[22] -> m_Val == "FIT"
           && x . m_Table[22] -> m_NextHash -> m_Key == "xns"
           && x . m_Table[22] -> m_NextHash -> m_Val == "test"
           && x . m_Table[22] -> m_NextHash -> m_NextHash -> m_Key == "asa"             
           && x . m_Table[22] -> m_NextHash -> m_NextHash -> m_Val == "butterfield"
           && x . m_FirstOrder == x . m_Table[1]
           && x . m_FirstOrder -> m_NextOrder == x . m_Table[45]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22] -> m_NextHash -> m_NextHash
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22] -> m_NextHash
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL
           && x . m_LastOrder == x . m_Table[22]
           && x . m_LastOrder -> m_PrevOrder == x . m_Table[22] -> m_NextHash
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[22] -> m_NextHash -> m_NextHash
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[45]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[1]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( n . m_Table[22] -> m_Key == "xns"
           && n . m_Table[22] -> m_Val == "test"
           && n . m_Table[22] -> m_NextHash -> m_Key == "asa"             
           && n . m_Table[22] -> m_NextHash -> m_Val == "butterfield"
           && n . m_FirstOrder == n . m_Table[1]
           && n . m_FirstOrder -> m_NextOrder == n . m_Table[45]
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder == n . m_Table[22] -> m_NextHash
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == n . m_Table[22]
           && n . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL 
           && n . m_LastOrder == n . m_Table[22]
           && n . m_LastOrder -> m_PrevOrder == n . m_Table[22] -> m_NextHash
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder == n . m_Table[45]
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == n . m_Table[1]
           && n . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( x . Del ( "xns" )
           && x . m_Table[22] -> m_Key == "whf"
           && x . m_Table[22] -> m_Val == "FIT"
           && x . m_Table[22] -> m_NextHash -> m_Key == "asa"             
           && x . m_Table[22] -> m_NextHash -> m_Val == "butterfield"
           && x . m_FirstOrder == x . m_Table[1]
           && x . m_FirstOrder -> m_NextOrder == x . m_Table[45]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22] -> m_NextHash
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL
           && x . m_LastOrder == x . m_Table[22]
           && x . m_LastOrder -> m_PrevOrder == x . m_Table[22] -> m_NextHash
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[45]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[1]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( x . Del ( "whf" )
           && x . m_Table[22] -> m_Key == "asa"             
           && x . m_Table[22] -> m_Val == "butterfield"
           && x . m_FirstOrder == x . m_Table[1]
           && x . m_FirstOrder -> m_NextOrder == x . m_Table[45]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder == x . m_Table[22]
           && x . m_FirstOrder -> m_NextOrder -> m_NextOrder -> m_NextOrder == NULL
           && x . m_LastOrder == x . m_Table[22]
           && x . m_LastOrder -> m_PrevOrder == x . m_Table[45]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder == x . m_Table[1]
           && x . m_LastOrder -> m_PrevOrder -> m_PrevOrder -> m_PrevOrder == NULL );
  assert ( x . IsSet ( "asa" ) );
  assert ( x . IsSet ( "xns" ) == false );
  assert ( x . IsSet ( "whf" ) == false );
  assert ( x . Del ( "asa" )
           && x . m_Table[22] == NULL );
  assert ( x . Del ( "peg" )
           && x . m_Table[1] == NULL );
  assert ( x . Del ( "bwc" )
           && x . m_Table[45] == NULL );
  assert ( x . m_FirstOrder == NULL
           && x . m_LastOrder == NULL );
  return 0;
}


