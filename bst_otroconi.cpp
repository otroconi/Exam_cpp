#include <iostream>
#include <utility> //move semnatics and std::pair
#include <memory> //unique ptr
#include <iterator>
#include <iomanip>
#include <vector>
#include <cstddef>
#include <chrono>
#include <random>
#include <algorithm> //std::sort()

template <typename T>
class node_t{
//	key_type _key; //remember here the videos with the animal class (when he mention about the snake, dogs (guau-guau))
	public:
	T  _value;
	std::unique_ptr<node_t> left;  //smart pointers
	std::unique_ptr<node_t> right; //smart pointer
	node_t * _parent;                        //row pointer

//	public:


	node_t(const T& value, node_t * p):   //copy ctor
//		_value{value}, _parent{p} { std::cout << "l-value" << std::endl; } //used to debug
		_value{value}, _parent{p} {  }
	node_t(const T&& value, node_t * p):   //move ctor
//		_value{std::move(value)}, _parent{p} { std::cout << "r-value" << std::endl; } //used to debug
		_value{std::move(value)}, _parent{p} {  }

	node_t() = default; // RAII 
//	~node_t() = default; 	

};

template <typename T>
T* left_left(T* x){
	while(x->left)
		x = x->left.get();
	return x;

	if(!x){
		std::cout << "no root" << std::endl;
		return x;
	};
}

template <typename T>
T* left_left(const std::unique_ptr<T>& x){
	return left_left(x.get());
}

template <typename T>
T* right_child(T* curr){
	auto parent = curr->_parent;

	if(!curr){
		return curr;
	}

	while (parent) {
		if(parent->left.get() == curr){
			break;
		}
		curr = parent;                   //remember when we did for the linked list in c exam
		parent = parent->_parent;
	}
	return parent;
}

template <typename node_t, typename deref_t>
class _iterator{
	node_t* current;

	public:
	using value_type      = deref_t;
	using reference       = value_type&;
	using pointer         = value_type*;
	using difference_type = std::forward_iterator_tag;

	reference operator*() const noexcept { 
		return current->_value; }
	pointer operator->() const noexcept {
		return &**this; }
/*	//pre-increment
	_iterator& operator++() noexcept {
		current = current->right.get();  ///check it!!!!  //OJO: this did not print all the tree
		return *this;                                     //only print the last inserted tree!!! recordar que demoramos en darnos cuenta
	}
*/
	//pre-increment
	_iterator& operator++() noexcept {
		if (current->right)
			current = left_left(current->right);
		else
			current = right_child(current);    
		return *this;
	}
	//post-increment
	_iterator &operator++(int) noexcept {
		auto tmp{*this}; //no-recomended because requires more memory
		++(*this);
		return tmp;
	}

//	_iterator(node_t* n): current{n} { std::cout << "it ctor" << std::endl; }   //used: to debug 
	_iterator(node_t* n): current{n} {  }
	friend
		bool operator==(const _iterator& a, _iterator& b) {                //similar to the linked list
			return a.current == b.current;
		}
	friend 
		bool operator!=(_iterator& a,  _iterator& b) {        //similar to the linked list  //OJO: COMPLAINS IF WE WRITE const
			return !(a == b);
		}

};

template <typename key_type, typename value_type, typename PO = std::less<key_type> >

class bst{


	public:

		using pair_type = std::pair<const key_type, value_type>;
		using node      = node_t<pair_type>;	
		using iterator = _iterator<node, pair_type>;
		using const_iterator = _iterator<node, const pair_type>;

//		using iterator = _iterator<node_t<key_type, value_type>, pair_type>;
//		using const_iterator = _iterator<node_t<key_type, value_type>, pair_type>;


		//Begin
		iterator begin() noexcept { return private_begin(); }
		const_iterator begin() const noexcept { return private_begin(); } //check if I can do const_iterator(function)   CHECK!!
		const_iterator cbegin() const noexcept { return private_begin(); } //cbegin() always returns a const iterator to the beginning

		//End
		iterator end() noexcept { return nullptr; } //returns an iterator one pass the last elem
		const_iterator end() const noexcept { return nullptr; }
		const_iterator cend() const noexcept { return nullptr; } //returns a const_iterator one pass the last elem

		//Find
		iterator find(const key_type& x) noexcept { return private_find(x); };
		const_iterator find(const key_type& x) const noexcept { return private_find(x); };	

		//Insert
		std::pair<iterator, bool> insert(const pair_type& x) { return private_insert(x); } //OJO: here not to write noexcept
		std::pair<iterator, bool> insert(pair_type&& x) { return private_insert(std::move(x)); } //Ojo: here not to write noexcept 

		//Emplace
		template <class... Types>
		std::pair<iterator,bool> emplace(Types&&...args){ return insert({std::forward<Types>(args)...}); }

		//Move semantic
		//the default is okay
		bst(bst&& ) noexcept = default;
		bst& operator=(bst&&) noexcept = default;

		//Copy semantic
		//needed because unique_ptr does not support by default the deep copy
		bst(const bst &x): po_comp{x.po_comp} {
			if(x.root)
				root = root.reset(new node{x.root});  //new call the ctor, since root is a uniq ptr it automatically release the memory
//				root = x.root;
		};
		
		bst &operator=(const bst &x) {
			root.reset();
			auto tmp{x}; //copy ctor   
			*this = std::move(x); //move assigment
			return *this;
		}



		//Subscripting operator
		value_type& operator[](const key_type& x) { return private_subscripting(x); }  //like alberto's did in the linked list with insert function
		value_type& operator[](key_type&& x) { return private_subscripting(std::move(x)); } //using the forwarding reference

		//clear
		void clear() noexcept { root.reset(nullptr); }; //reset(ptr) = release + change the direction of the unique_ptr to the new ptr
		
		//balance
		void balance();

		//RAII
		bst(PO K): PO{K} {};      //ctor for the PO
		bst() noexcept = default; //the default generated ctor and desctor are
		~bst() noexcept = default; //fine because we do not have row pointers   //RAII

	private:

//	std::unique_ptr<node_t<key_type, value_type>>*  root;  //for two templated node

	std::unique_ptr<node>  root;  //for one templated node //OJO: recordar no colocar *
	PO po_comp;

	node* private_begin() const noexcept { return left_left(root); } //noexcept because we are not adquiring new resources //OJO: write const (if not compiler complains)
	node* private_find(const key_type& x) const noexcept {          //noexcept again (not adquiring resourses)
		auto tmp = root.get();  //get() return a row ptr
		const auto& tmp_key = tmp->_value.first;              
		if(!tmp){
			return nullptr;
		};

	        while (tmp) {

			if(po_comp(x,tmp_key)){                         //we compare both keys:the root key (tmp_key) and x (which is key_type already)
				tmp = tmp->left.get(); //search in the left
//				std::cout << "found in the left" << std::endl;
			} else if(po_comp(tmp_key,x)){
				tmp = tmp->right.get(); //search in the right
//				std::cout << "found in the right" << std::endl;
			} else if (x == tmp_key) {
				std::cout << "the keys are equal" << std::endl;
				break; // in this case they are equal ==> found it!
			};
		};
		return tmp;
	}

	template <typename O>
	value_type& private_subscripting(O&& x){
//		std::cout << "forwarding subscripting" << std::endl; //Forwarding reference must be templated
		auto it = find(x);                                   //they accept both l-value and r-value
		if(it != end()){                                     //usefull to avoid code repetition
			return it->second;
		}
		return emplace(std::forward<O>(x), value_type{}).first->second;
	};

	template <typename O>
	std::pair<iterator, bool> private_insert(O&& x );  //larger function ==> defined outside the class	

	void recursive_insertion(std::vector<pair_type>);

	//Put-to operator
	friend
		std::ostream &operator<<(std::ostream& os, const bst& x){
			for (const auto y: x){
//			for (iterator it = x.begin(); it != x.end(); ++it){			
				os << "key: " << y.first << "," << "value " << y.second << " ";
			}
			os << std::endl;
			return os;
		};

};

//Functions defined outside the class bst (hence too long)

//private_insert
/*
template <typename key_type, typename value_type, typename PO> //if we write PO=std::less<key_type> ==> error:default value for x
template <typename O>
std::pair<typename bst<key_type, value_type, PO>::iterator, bool>  //we cannot write only iterator, instead all the qualified name
bst<key_type, value_type, PO>::private_insert(O&& x ){            // i.e bst<key_type, value_type, OP>::iterator (se arrastra todo el nombre)
	
	bool my_bool=true;
//	node_t* new_node = new node_t{std::forward<O>(x)};
	auto tmp = root.get();
	auto &tmp_key = tmp->_value.first; //key of tmp (llave de la root)   //OJO: use & because if not its compile but with segmentation fault
	const auto &x_key = x.first;            //key of x (llave de la clase x), use const because the key should be const //same here: use &t 

	
	//if root is not present
	if(!root){
		root = std::make_unique<node>(x,nullptr); 
		return {root.get(), my_bool=true};
	};
	if(my_bool) {
 		if (tmp->left || tmp->right){
			if(x_key<tmp_key){
				tmp = tmp->left.get();
				return {tmp, my_bool = true};
			} else if(tmp_key<x_key){
				tmp = tmp->right.get();
				return {tmp, my_bool = true};
			} else {
				return {tmp, my_bool = true};
			}
		} else if ( !(tmp->left) || !(tmp->right) ) {
			if(x_key<tmp_key){
				tmp->left = std::make_unique<node> (std::forward<O>(x), tmp);
				return {tmp->left.get(), my_bool = true};
			} else if(tmp_key<x_key){
				tmp->right = std::make_unique<node>(std::forward<O>(x), tmp);
				return {tmp->right.get(), my_bool = true};
			} else {
				return {tmp, my_bool = true};
			}
		}
		return {tmp,my_bool};
	} else {
		return {tmp, my_bool = false};
	};
//	return {tmp, my_bool};
				
}	
*/
template <typename key_type, typename value_type, typename PO> //if we write PO=std::less<key_type> ==> error:default value for x
template <typename O>
std::pair<typename bst<key_type, value_type, PO>::iterator, bool>  //we cannot write only iterator, instead all the qualified name
bst<key_type, value_type, PO>::private_insert(O&& x ){            // i.e bst<key_type, value_type, OP>::iterator (se arrastra todo el nombre)

	bool my_bool = true;
	if (!root) {
		root = std::make_unique<node>(std::forward<O>(x), nullptr);
    	return {root.get(), my_bool=true};
  	}

  	auto tmp        = root.get();
  	auto& tmp_key = tmp->_value.first;
  	auto& x_key = x.first;
/*   while (tmp_key==x_key){
	   return {tmp, false};
   };
*/
  	while (true) {

    		if (x_key < tmp_key) {

      			if (!tmp->left) {
        		tmp->left = std::make_unique<node>(std::forward<O>(x), tmp);
        			return {tmp->left.get(), true};
      			} else {
        			tmp = tmp->left.get();
			//	return {tmp, true};
      			}

    		} else if (tmp_key < x_key ) {

      			if (!tmp->right) {
        		tmp->right = std::make_unique<node>(std::forward<O>(x), tmp);
        			return {tmp->right.get(), true};
      			} else {
        			tmp = tmp->right.get();
			//	return {tmp, true};
      			}

    		} else if (x_key == tmp_key){
			//	    break;
		      return {tmp, false};
    		}
  	}
}


template <typename key_type, typename value_type, typename PO> //if we write PO=std::less<key_type> ==> error:default value for x
void bst<key_type, value_type, PO>::recursive_insertion(std::vector<pair_type>){
	std::vector<pair_type> pairs;
	std::size_t size = pairs.size();
	std::size_t middle = 0.5 * pairs.size(); 

	int i;
	if(!size%2)
		i = middle;                                   //Not completed! :(
	else 
		i = middle -1;

	std::vector<pair_type> pairs2 = pairs[i]; 
//	std::vector<pair_type> pairs2_left_o = pairs2.begin();
//	std::vector<pair_type> pairs2_left_f = pairs2.end();	

	
	
}

template <typename key_type, typename value_type, typename PO> //if we write PO=std::less<key_type> ==> error:default value for x
void bst<key_type, value_type, PO>::balance(){
	std::vector<pair_type> pairs;

	auto begin_pair = begin();
	auto end_pair  = end();

	while(!root){
		break;
	};

	for (iterator it = begin_pair; it != end_pair; ++it){
		pairs.push_back(*begin_pair);   //remember here when he discussed in the vector excersise session that push_back
	}                                      //takes only one argument 

	clear();        //Remember it!!

	std::sort(begin_pair,end_pair);
	recursive_insertion(pairs);
}

int main(){

	std::cout << "Starting the class node check" << std::endl;

	node_t<int> N{0, nullptr};
	node_t<int> N2{3, nullptr};

	std::cout << "Starting the class bst check" << std::endl;
	bst<int,int> tree{};
//	std::cout << tree << std::endl; //this returns segmentation fault

	std::cout << "Prove insert" << std::endl;

	std::cout <<"--------First insert-----------" << std::endl;
	tree.insert(std::pair<int,int> (15,5));
	std::cout << tree << std::endl;

	std::cout <<"--------Second insert-----------" << std::endl;
	tree.insert(std::pair<int,int> (10,2));  
	std::cout << tree << std::endl;

	std::cout << "testing iterator" << std::endl;
	bst<int,int>::iterator it = tree.begin();

	++it;
	

	std::cout <<"--------Third insert-----------" << std::endl;
	tree.insert(std::pair<int,int> (35,1));  
	std::cout << tree << std::endl;

	std::cout << "Prove emplace" << std::endl;
	tree.emplace(14,6); 

	std::cout << tree << std::endl;

	std::cout << "Prove emplace again" << std::endl;
	tree.emplace(13,11);

	std::cout << tree << std::endl;

	std::cout <<"--------More inserts-----------" << std::endl;
	tree.insert(std::pair<int,int> (45,6));

	std::cout << tree << std::endl;

	
	tree.insert(std::pair<int,int> (19,7)); 

	tree.insert(std::pair<int,int> (5,26));

	tree.insert(std::pair<int,int> (5,26));
	std::cout << tree << std::endl;



	std::cout << "testing iterator" << std::endl;
	bst<int,int>::iterator it3 = tree.begin();

	++it3;

	std::cout << " printing the first element of the tree to test .begin() " << "\n" <<
		"the first key, value of the list is: ( " << it3->first << "," << it3->second << " )" << std::endl;
	
//	bst<int,int>::iterator it4 = tree.end();

//	std::cout << " printing the last element of the tree to test .end() " << "\n" <<
//		"the first key, value of the list is: ( " << it4->first << "," << it4->second << " )" << std::endl; //segmentation fault
//	++it4;

//	++it4; //it gives you segmentation fault ==> it is ok

	std::cout << "testing find key=10" << std::endl;
	auto t_o = std::chrono::high_resolution_clock::now();
	tree.find(10);
	auto t_f = std::chrono::high_resolution_clock::now();

	auto delta_t = std::chrono::duration_cast<std::chrono::nanoseconds>(t_f - t_o);

	std::cout << "time to find the key=10 is " << delta_t.count() << "nanos" << std::endl;

//	std::cout << " printing the first element of the tree to test .find() " << "\n" <<
//		"the first key, value of the list is: ( " << it5->first << "," << it5->second << " )" << std::endl; //==>segmentation fault

//	++it5; //==>segmentation fault

	tree.find(19);	
//	std::cout << tree.find(19)->first << std::endl; //segmentation fault???????????

	std::cout << "testing find key=100" << std::endl;
	auto t_oa = std::chrono::high_resolution_clock::now();
	tree.find(100);
	auto t_fa = std::chrono::high_resolution_clock::now();

	auto delta_ta = std::chrono::duration_cast<std::chrono::nanoseconds>(t_fa - t_oa);

	std::cout << "time to find the key=100 is " << delta_ta.count() << "nanos" << std::endl;


//	++it6;

	std::cout << "testing find key=5" << std::endl;
	auto t_ob = std::chrono::high_resolution_clock::now();
	tree.find(5);
	auto t_fb = std::chrono::high_resolution_clock::now();

	auto delta_tb = std::chrono::duration_cast<std::chrono::nanoseconds>(t_fb - t_ob);

	std::cout << "time to find the key=5 is " << delta_tb.count() << "nanos" << std::endl;

//	++it7;
//	it7++; //test the post-increment // Warning: reference to local variable tmp, COMPILE COMPLAINS 

	std::cout << "printing the tree again" << std::endl;
	std::cout << tree << std::endl;

	std::cout << "testing find key=300" << std::endl;
	auto t_oc = std::chrono::high_resolution_clock::now();
	tree.find(300);
	auto t_fc = std::chrono::high_resolution_clock::now();

	auto delta_tc = std::chrono::duration_cast<std::chrono::nanoseconds>(t_fc - t_oc);

//	++it8;

	std::cout << "time to find the key=300 is " << delta_tc.count() << "nanos" << std::endl;

	std::cout << "Bigger tree" << std::endl;
	
	bst<int,int> tree2{};
	for (int i = 0; i < 20; ++i){
		tree2.insert(std::pair<int,int>(i,2));
	}

	std::cout << tree2 << std::endl;

	std::cout << "testing iterator for bigger list" << std::endl;
	bst<int,int>::iterator it_tree2 = tree2.begin();

	std::cout << " Big tree: printing the first element of the tree to test .begin() " << "\n" <<
		"the first key, value of the list is: ( " << it_tree2->first << "," << it_tree2->second << " )" << std::endl;

	++it_tree2;
	

	std::cout << " Big tree: printing the first element of the tree to test ++iterator " << "\n" <<
		"the first key, value of the list is: ( " << it_tree2->first << "," << it_tree2->second << " )" << std::endl;

	tree2.clear();
	tree.clear();

//	std::cout << "after tree.clear() " << tree2 << std::endl; //segmentation fault ==> It is ok

/*
	for (auto it_tree2  = tree2.begin(); it_tree2 != tree2.end(); ++it_tree2) {  //complains about !=
		
		std::cout << " ( " << it_tree2->first << "," << it_tree2->second << " )" << std::endl;
	}; 
*/


	return 0;


}


