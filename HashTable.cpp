#include <iostream>
#include <cstddef>
#include <string>
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;

template <typename KEY, typename VALUE>
class Entry
{
	template<typename K, typename V> friend class HashTable;

	private:
		KEY key;
		VALUE value;
		Entry * next_entry;

	public:
		/*** Constructors ***/
		Entry(void) : key(KEY()), value(VALUE()), next_entry(NULL) {}
		//If you would like you can convert constructors to have this look
		Entry(KEY key, VALUE value)
		{
			this->template key = key;
			this->template value = value;
			this->template next_entry = NULL;
		}
		Entry(KEY key, VALUE value, Entry * next) : key(key), value(value), next_entry(next) {}

		//accessors
		KEY get_key() { return key; }
		VALUE get_value() { return value; }
		Entry * get_next_entry() { return next_entry; }

		//mutators
		void set_key(KEY key) { this->key = key; }
		void set_value(VALUE value) { this->value = value; }
		void set_next_entry(Entry * entry) { this->next_entry = entry; }

};

template<typename K, typename V>
class HashTable
{
	private:
		int N;
		Entry<K,V> **hash_table; //Make sure you understand this line
		bool isResizing; //Used in insert, when resizing
		int timesSinceCalled; //Also used in insert, when resizing

	public:
		HashTable()
		{
			this->N = 10000;
			hash_table = new Entry<K, V> * [this->N];

            for (int i = 0; i < this->N; i++)
				hash_table[i] = NULL;
			
			isResizing = false;
			timesSinceCalled = 0;
		}

		HashTable(int N)
		{
			this->N = N;
           	 	hash_table = new Entry<K, V> * [this->N];

            	for (int i = 0; i < this->N; i++)
					hash_table[i] = NULL;
			
				isResizing = false;
				timesSinceCalled = 0;
		}

		/* hashcose string to int */
		int hashcode(string s) {
			int hash_value = 0; //Value you are returning
			for (int i = 0; i < s.length(); i++) {
				hash_value = (127 * hash_value + s.at(i)) % 16908799; //Charat or at?
			}
			return hash_value;
		}

		//hashcode int to int 
		int hashcode(int i) {
			int hash_value = i;
			//Algorithms to create hash codes for ints
			//Modding by same prime number as was used to make hash codes from strings
			if (i % 2 == 0) {
				hash_value = (1011 * (hash_value / 5) + (hash_value * 879)+1983) % 16908799;
			}
			if (i % 3 == 0) {
				hash_value = (1031 * (hash_value / 7) + (hash_value * 597)+3124) % 16908799;
			}
			if (i % 6 == 0) {
				hash_value = (1001 * (hash_value / 9) + (hash_value * 231)+1329) % 16908799;
			}
			else {
				hash_value = (1007 * (hash_value / 3) + (hash_value * 1071)+2451) % 16908799;
			}
			if (hash_value < 0) {
				hash_value = abs(hash_value);
			}
			return hash_value;
			
		}
		 
		// hascode char to int
		int hashcode(char c) {
			//Algorithms to create hash codes for strings
			//Again modding by same prime number as was used to make hash codes from strings
			//Initially modifying keys
			int hash_value = c;
			if (c % 2 == 0) {
				hash_value *= 135;
			}
			if (c % 3 == 0) {
				hash_value += 137;
			}
			if (c % 5 == 0) {
				hash_value += 6;
			}
			if (c % 6 == 0) {
				hash_value *= 79;
			}
			if (c % 7 == 0) {
				c += 183;
			}
			
			//Modifying all keys
			hash_value *= 19;
			hash_value + 531;

			//Using now previously modifed keys to make hash codes
			//Using same algorithms as ints
			if (c % 2 == 0) {
				hash_value = (1011 * (hash_value / 5) + (hash_value * 879) + 1983) % 16908799;
			}
			if (c % 3 == 0) {
				hash_value = (1031 * (hash_value / 7) + (hash_value * 597) + 3124) % 16908799;
			}
			if (c % 6 == 0) {
				hash_value = (1001 * (hash_value / 9) + (hash_value * 231) + 1329) % 16908799;
			}
			else {
				hash_value = (1007 * (hash_value / 3) + (hash_value * 1071) + 2451) % 16908799;
			}
			if (hash_value < 0) {
				hash_value = abs(hash_value);
			}
			return hash_value;
		}
		
		// hascode long to int
		int hashcode(unsigned long ul) {
			int hash_value = ul;
			//Algorithms to create hash codes for unsigned longs
			//Again modding by same prime number as was used to make hash codes from strings
			//Making values larger because unsigned longs can hold larger values than ints
			if (ul % 2 == 0) {
				hash_value = (21908 * (hash_value / 5) + (hash_value * 879) + 124536) % 16908799;
			}
			if (ul % 3 == 0) {
				hash_value = (98746 * (hash_value / 7) + (hash_value * 597) + 7382646) % 16908799;
			}
			if (ul % 6 == 0) {
				hash_value = (1789 * (hash_value / 9) + (hash_value * 231) + 172) % 16908799;
			}
			else {
				hash_value = (24371 * (hash_value / 3) + (hash_value * 1071) + 7) % 16908799;
			}
			if (hash_value < 0) {
				hash_value = abs(hash_value);
			}
			return hash_value;
		}

		// basic compression function 
		int compression(int hc) {
			return ((5 * hc + 11) % 16908799) % N;
		}

		// insert e into hash table 
		void insert(Entry<K, V> * e) {
			//Entry<K, V>* entry1;
			//Finding the hashcode of the entry (e)'s key
			//Compressing/hashing that hashcode
			//Storing this value into bucket (the number stored is the bucket the netry is being stored  in
			int bucket = compression(hashcode(e->get_key()));

			//If the bucket is empty, put the entry into the bucket
			if (hash_table[bucket] == NULL){
				hash_table[bucket] = e;
			}
			
			//Else, if the bucket is not empty, put the entry into a chain off of the bucket
			
			else {
				e->set_next_entry(hash_table[bucket]);
				hash_table[bucket] = e;
				
				/* entry1 = hash_table[bucket];
				while (entry1->get_next_entry() != NULL) {
					entry1 = entry1->get_next_entry();
				}
				entry1->set_next_entry(e); */
			} 

			
			//Calling resize function
			double loadFactor; 
			if (timesSinceCalled > (N/2)) {
				if (isResizing == false) {
					//Ideal Load Factor is 0.8-1.2
					//Growing in load factor is greater than 1.5 becuase load factor has a linear time complexity
					//Due to how costly the resize operation is, there is a small gap between the ideal load factor
					//and the load factor it is being checked at
					loadFactor = compute_load_factor();
					if (loadFactor > 1.5) {
						resize(1);
						timesSinceCalled = 0;
					}
				}
				
			}
			timesSinceCalled++;
		}
		
			
		// replace e1 with e2 
		void replace(Entry<K, V> * e1, Entry<K, V> * e2) {
			Entry<K, V>* temp = find(e1); //Find (e1) returns the address of e1
			Entry<K, V>* previousToTemp = find_previous(temp);
			Entry<K, V>* tempNextEntry = temp->get_next_entry();
			
			int bucket = -1;
			//If the entry is not in the list, return
			if (temp == NULL) {
				cout << "ERROR: Entry 1 is not in the function. Therefore, entry 1 was not replaced with entry 2." << endl;
				return;
			}
		

			else {
				//If temp is pointing to the last element in the list, but is not the head
				if (tempNextEntry == NULL && previousToTemp != NULL) {
					previousToTemp->set_next_entry(NULL);
				}
				//If temp is pointing to an element in the middle of the list 
				//(not the head or the last element in the list)
				else if (tempNextEntry != NULL && previousToTemp != NULL) {
					previousToTemp->set_next_entry(tempNextEntry);
				}
				//If temp is pointing tot he head of the list, and there ARE other elements in the list
				else if (tempNextEntry != NULL && previousToTemp == NULL) {
					//Finding hashcode of e1's key and compressing it
					bucket = compression(hashcode(e1->get_key()));
					hash_table[bucket] = tempNextEntry;
				}
				//If temp is pointing to the head of the list, and there are no other elements in the list,
				//No extra steps are required
				delete temp; //Deletes what temp is pointing to
				temp = e2; //Temp is now pointing to e2
				insert(temp); //Inserts temp into hash table (temp is pointing to the address of e2,
							  //so e2 is inserted into the hash table)
			}
		}
		

		
		//find e and return 
		Entry<K, V> * find(Entry<K, V> * e) {
			if (e == NULL) {
				return NULL;
			}
			//Finding hashcode of e's key and compressing it
			int bucket = compression(hashcode(e->get_key()));

			//Creating an Entry pointer to use as a cursor
			//Entry pointer initially points to head of list at bucket passed in
			Entry<K, V>* entry1 = hash_table[bucket];

			//If the entry is not found, return NULL
			
			if (entry1 == NULL)
			{
				return NULL;
			}
			
			//If the entry is the "head" bucket return that entry
			else if ((entry1->get_key() == e->get_key()) && (entry1->get_value() == e->get_value())) {
				return hash_table[bucket];
			}
			//else loop through list extending from that bucket until the entry is found, then return the entry
			else {
				entry1 = entry1->get_next_entry();
				while (entry1 != NULL) {
					//Return entry if found
					if ((entry1->get_key() == e->get_key()) && (entry1->get_value() == e->get_value())) {
						return entry1;
					}
					//else set entry1 to next entry in the list
					else {
						entry1 = entry1->get_next_entry();
					}
				}
			}
			//If the entry was not found in any of the buckets, return NULL
			return NULL;
		}
		
	
		// remove e from the table 
		 void remove(Entry<K, V> * e) {
			//(Using pieces from find function)
			//Finding hashcode of e's key and compressing it
			int bucket = compression(hashcode(e->get_key()));

			//Creating an Entry pointer to use as a cursor
			//Entry pointer initially points to head of list at bucket passed in
			Entry<K, V>* entry1 = hash_table[bucket];
			Entry<K, V>* entry2 = NULL;
			//If there is nothing in the bucket, return
			if (entry1 == NULL) {
				cout << "This entry does not exist. " << endl; 
				return;
			}
			//If the entry is the "head" bucket, make the next entry the "head" of the list
			//Then delete entry1 (the element that matches what was passed into the function
			else if ((entry1->get_key() == e->get_key()) && (entry1->get_value() == e->get_value())) {
				//Entry1 is already equal to hash_table[bucket] (acts as head of the list)
				hash_table[bucket] = entry1->get_next_entry();
				delete entry1;
				return;
			}

			//else loop through list extending from that bucket until the entry is found
			else {
				entry1 = entry1->get_next_entry();
				while (entry1 != NULL) {
					//If entry is found, set entry1's previous entry equal to (pointing to) entry1's next entry
					//Then delete entry1
					if ((entry1->get_key() == e->get_key()) && (entry1->get_value() == e->get_value())) {
						entry2=find_previous(entry1);
						entry2->set_next_entry(entry1->get_next_entry()); //Check?
						delete entry1;
					}
					//else set entry1 to next entry in the list
					else {
						entry1 = entry1->get_next_entry();
					}
				}
			}

			//Calling resize function
			double loadFactor;
			if (timesSinceCalled > (N / 2)) {
				if (isResizing == false) {
					//Ideal Load Factor is 0.8-1.2
					//Growing in load factor is greater than 1.5 becuase load factor has a linear time complexity
					//Due to how costly the resize operation is, there is a small gap between the ideal load factor
					//and the load factor it is being checked at
					loadFactor = compute_load_factor();
					if (loadFactor < 0.5) {
						resize(0);
						timesSinceCalled = 0;
					}
				}

			}
			timesSinceCalled++;
		}
		
		// if grow = 1 increase the table
 		 // otherwise decrease the table
 		 //
		 void resize(bool grow) {
			 //Creating a flag to check if the table is still resizing (declared as member variable)
			isResizing = true; 
			 ///Growing
			 Entry<K, V>** temp = NULL;
			 Entry<K, V>** temp2 = NULL;
			 Entry<K, V>*e1 = NULL;
			 Entry<K, V>*next = NULL;
			 int oldN = N;
			 if (grow == 1) {
				 N = 2 * N; //Doubling the size of the list to grow
			 }

			 else {
				 if (N <= 1) {
					 cout << "There is only one element in the hash table. Cannot shrink. " << endl;
					 return;
				 }
				 N = N / 2; //Halfing the size of the list to shrink
			 }

				 temp = new Entry<K, V>*[N]; //Creates a new dynamic array
				 temp2 = hash_table; //temp2 now points to the original hash_table, which has the elements you want to move into the new array
				 hash_table = temp; //hash_table now points to temp (the currently empty array that is double the size of hash_table when this function was called
				
				 for (int i = 0; i < oldN ; i++) { 
					 e1 = temp2[i];
					 while (e1 != NULL) {
						 next = e1->get_next_entry();
						 e1->set_next_entry(NULL);
						 insert(e1);
						 e1 = next;
					 }
					 temp2[i] = NULL;
				 }
				 isResizing = false;
		}
		
		 double compute_load_factor() {
			 //Load factor is n/N
			 //N is already computed
			 //Need to find n
			
			 Entry<K, V>* entry1 = NULL; //Creating a pointer to an entry
			 double n = 0; //Counts number of elements in the hash table (n)
			 double load_factor;
			 if (N == 0) {
				//If the denominator = 0, the quotient would be undefined
				 cout << "ERROR: There were 0 buckets. Therefore, the load factor would compute an undefined number.";
				 cout << "-1 has been returned." << endl;
				 return -1;
			 }
			 //Calculating n
			 for (int i = 0; i < N; i++) {
				 //Setting entry1 to the head of the linked list of bucket i
				 entry1 = hash_table[i];
				 //Moving to next element in the list and keeping track of n
				 while (entry1!=NULL) {
					 n++;
					 entry1 = entry1->get_next_entry();
				 }	 
			 }
			 load_factor = n / N;
			 return load_factor;
		}

		 int longest_chain_length() {
			 Entry<K, V>* entry1 = NULL; //Creating a pointer to an entry
			 int chainLengthCounter = 0;
			 int maxChain = 0;
			 for (int i = 0; i < N; i++) {
				 //Setting entry1 to the head of the linked list of bucket i
				 entry1 = hash_table[i];
				 //Moving to next element in the list
				 while (entry1 != NULL) {
					 chainLengthCounter++;
					 entry1=entry1->get_next_entry();
				 }
				 if (chainLengthCounter > maxChain) {
					 maxChain = chainLengthCounter;
				 }
				 chainLengthCounter = 0;
			 }
			 return maxChain;
		} 
	

		//FINDING PREVIOUS
		Entry<K, V>* find_previous(Entry<K, V>* e) {
			//Finding hashcode of e's key and compressing it
			int bucket = compression(hashcode(e->get_key()));
			//Creating an Entry pointer to use as a cursor
			//Entry pointer initially points to head of list at bucket passed in
			Entry<K, V>* entry1 = hash_table[bucket];
			//If there is not a previous entry (if e is the head of the list), return NULL
			if ((entry1->get_key() == e->get_key()) && (entry1->get_value() == e->get_value())) {
				return NULL;
			}
			else {
				entry1 = entry1->get_next_entry();
				while (entry1 != NULL) {
					//Return entry if found
					if ((entry1->get_key() == e->get_key()) && (entry1->get_value() == e->get_value())) {

					}
					//else set entry1 to next entry in the list
					else {
						while (entry1->get_next_entry()->get_key() != e->get_key() && entry1->get_next_entry()->get_value() != e->get_value()) {
							entry1 = entry1->get_next_entry();
						}
						//Returns entry1, which is the previous entry to e
						return entry1;
					}
				}
			}


			
		}
		//Deconstructor
		~HashTable() {
			Entry<K, V>* e1;
			Entry<K, V>* prev = NULL;
			for (int i = 0; i < N; i++) {
				e1 = hash_table[i];
				while (e1 != NULL) {
					prev = e1;
					e1 = e1->get_next_entry();
					delete prev;
				}
			}
			delete hash_table;
		} 
		


		//DEBUG FUNCTIONS
		void debug1() {
			
			int bucket = compression(hashcode(static_cast <unsigned long> (34)));
			Entry<K, V>* current;
			current = hash_table[bucket];
			while (current!=NULL){
				cout << "The memory address of current is " << current << endl;
				current = current->get_next_entry(); //Check later!!!! (get/set)
				
			}
		}

		//Good to use to check resize
		//Similar to longestchain
		//Run before and after resize is called
		void debug2() {
			Entry<K, V>* entry1 = NULL; //Creating a pointer to an entry
			int chainLengthCounter = 0;
			int maxChain = 0;
			for (int i = 0; i < N; i++) {
				//Setting entry1 to the head of the linked list of bucket i
				entry1 = hash_table[i];
				//Moving to next element in the list
				while (entry1 != NULL) {
					chainLengthCounter++;
					entry1 = entry1->get_next_entry();
				}
				if (chainLengthCounter > maxChain) {
					maxChain = chainLengthCounter;
				}
				cout << "The bucket " << i << " has " << chainLengthCounter << " entries. " << endl;
				chainLengthCounter = 0;
			}
			
		}
};

/* I recommend you make a temp main for testing all of your boundary
 * cases.  I reserve the right to change the main function.  I promise
 * it will only call the function prototypes provided; which means, you
 * cannot change the prototypes.
 */


//Prof's main method
int main()
{
	//seed the random number generator
	srand(42);

	HashTable<int, string>  table_01(10000);
	int key;
	string value;
	Entry<int, string> * e;

	// Fill the table with random entries
	for (int i = 0; i < 100000; i++)
	{
		/* create a random entry */
		//key = (sizeof(int) < sizeof(long)) ? (static_cast<int>(((unsigned long)rand()) << (sizeof(int) * 8)) | rand()) : rand();
		value = "";
		key = rand() % 119984135204;
		for (int j = 0; j < (rand() % 45 + 1); j++)
			value += 'a' + rand() % 26;
		//key = value;
		e = new Entry<int, string>(key, value);
		if (i % 10000 == 0) {
			cout << "Currently at element " << i << endl;
		}
		table_01.insert(e);
	}

	cout << "Longest Chain: " << table_01.longest_chain_length() << endl;
	cout << "Load Factor: " << table_01.compute_load_factor() << endl;

	return 0;
}