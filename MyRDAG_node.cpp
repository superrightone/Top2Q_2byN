//MyRDAG_node.cpp

#include "MyRDAG_node.h"
#include <assert.h>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <ratio>

MyRDAG_node::MyRDAG_node() :
    left_range{0}, right_range{0}, weight{0}, level{0}
	, opt_lx{-1}, opt_l{-1}, opt_rx{-1}, opt_r{-1}
    , l_child{NULL}, r_child{NULL} 
	, l_parent{NULL}, r_parent{NULL}
    , ref{0}, leaf{false}   {

}

MyRDAG_node::MyRDAG_node(int l, int r, bool w) :
	left_range{l}, right_range{r}, weight{w}, level{0}
	, opt_lx{-1}, opt_l{-1}, opt_rx{-1}, opt_r{-1}
    , l_child{NULL}, r_child{NULL} 
	, l_parent{NULL}, r_parent{NULL}
	, ref{0}, leaf{false}  {

}

MyRDAG_node::MyRDAG_node(int l, int r, bool w, 
		                 MyRDAG_node* l_pa, MyRDAG_node* r_pa, node_ref refer) :
	 left_range{l}, right_range{r}, weight{w}, level{0}
	 ,opt_lx{-1}, opt_l{-1}, opt_rx{-1}, opt_r{-1}
	 , l_child{NULL}, r_child{NULL}, l_parent{l_pa}, r_parent{r_pa}{
	
	this->ref = refer;
    if(r - l <= 0 ){
		weight = false;
		leaf = true;
	}
	else
		leaf = false;

}
/**MyRDAG_node member function END**/

MyRDAG_node::MyRDAG_node(int l, int r,
                          MyRDAG_node* l_pa, MyRDAG_node* r_pa, node_ref refer) :
      left_range{l}, right_range{r}, weight{false}, level{0}
	  , opt_lx{-1}, opt_l{-1}, opt_rx{-1}, opt_r{-1}
      , l_child{NULL}, r_child{NULL}, l_parent{l_pa}, r_parent{r_pa}{

     this->ref = refer;
     if(r - l <= 0 ){
         weight = false;
         leaf = true;
     }
     else
         leaf = false;
}


MyRDAG_node::~MyRDAG_node(){

}


/*************************************************************************************************************
 *
 *************************************************************************************************************/
void MyRDAG_node::build(){
  static int b_count = 0;
  b_count++;
  //cerr << "COUNT: " << b_count << endl;
  //chrono::steady_clock::time_point ta = chrono::steady_clock::now();
  (*ref.node_map)[pair<int, int>(left_range, right_range)] = this;
  //chrono::steady_clock::time_point tb = chrono::steady_clock::now();
  //cerr << "check point 0 : " << chrono::duration_cast<chrono::nanoseconds>(tb - ta).count() << endl;
  assert(right_range - left_range >= 0);
  if(right_range - left_range == 0){
    this->weight = false;
	this->total_weight = 1;
    this->leaf = true;
    *(ref.weight_bits) += this->total_weight;
	//cerr << "address of leaf: " << this << endl;
	return;
  }
  chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
  //int y1, y2;
  //this->get_opt_cols(&y1, &y2);
  this->get_opt_cols();
  //cerr << "opt cols: " << opt_l <<", " << opt_r << endl;
  chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
  //cerr << "check point 1 : " << chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() << endl;
  bool lw, rw;
  lw = rw = false;
  if(this->opt_l == this->opt_r){
    if(left_range < (this->opt_r - 1) )
		lw = true;
	if(right_range > (this->opt_l + 1) )
		rw = true;
	else{
	  lw = rw = false;	
	}
  }
  else{
    lw = rw = false;
  }

  
  /**Checking duplicate node and building left child**/
  chrono::steady_clock::time_point t3 = chrono::steady_clock::now();
  MyRDAG_node* compare = expect_duplicate(left_range, this->opt_r - 1);
  
  chrono::steady_clock::time_point t4 = chrono::steady_clock::now();
  //cerr << "check point 2 : " << chrono::duration_cast<chrono::nanoseconds>(t4 - t3).count() << endl;
  //if(l_parent && l_parent->l_child && l_parent->l_child->r_child){
  //chrono::steady_clock::time_point t5 = chrono::steady_clock::now();
  if(compare){
	l_child = compare;
	l_child->r_parent = this;
	*(ref.weight_bits) -= l_child->total_weight;
	l_child->weight = l_child->check_real_weight();
	this->l_child->total_weight = l_child->weight ? 2 : 1;
	*(ref.weight_bits) += l_child->total_weight;
	//cerr << "New update weight bit count: " <<*(ref.weight_bits) << endl;;
  }
  else{
    if(left_range < this->opt_r ){
//		chrono::steady_clock::time_point t5_1 = chrono::steady_clock::now();
		l_child = new MyRDAG_node(left_range, this->opt_r - 1, lw, NULL, this, ref);
//		chrono::steady_clock::time_point t5_2 = chrono::steady_clock::now();
    	l_child->build();
	}
	else{
	}
  }

  if( this->opt_l < right_range){
//	  chrono::steady_clock::time_point t5_1 = chrono::steady_clock::now();
	  r_child = new MyRDAG_node(this->opt_l + 1, right_range, rw, this, NULL, ref);
//	  chrono::steady_clock::time_point t5_2 = chrono::steady_clock::now();
      r_child->build();
  }
  else{
	  //cout << "right build banned due to range" << endl;
  }
  //if(!leaf) get_opt_cols(&opt_l, &opt_r);
//  chrono::steady_clock::time_point t6 = chrono::steady_clock::now();

  this->weight = this->check_real_weight();
  if(!l_parent && !r_parent) this->weight = true;

  this->total_weight = this->weight ? 2 : 1 ;
  *(ref.weight_bits) += this->total_weight;
  //cerr << "weight bit count: " <<*(ref.weight_bits) << " - " ;
  //if(l_parent) cerr << l_parent->weight << " - " ;
  //if(r_parent) cerr << r_parent->weight ;
  //cerr << endl;
  #ifdef DEBUG 
    cerr << "Build : " <<  total_weight <<", "<< "range : " << left_range << ", " << right_range << endl;
    if(!leaf) cerr << "opt cols: " << y1 << ", " << y2 << endl;
  #endif
}

/*************************************************************************************************************
 *vector<int>* MyRDAG_node::top2Q
 *
 * param: int x1, int y1, int x2, int y2
 *************************************************************************************************************/
vector<int>* MyRDAG_node::top2Q(int x1, int y1, int x2, int y2){
  int a = (y1 < y2) ? y1 : y2;
  int b = (y1 < y2) ? y2 : y1;
  int top = -1;
  int top2 = -1;
  //FourTopk_second_oned* A1 = ref.A1; 
  //FourTopk_second_oned* A2 = ref.A2;
  vector<int>* list1 = ref.list1;
  vector<int>* list2 = ref.list2;

  vector<int>* cand = new vector<int>();
  vector<int>* cand2 = new vector<int>();
  if(left_range <= a && b <= right_range){
	  if(a <= opt_l && opt_r <= b){
		  return _top2Q();
	  }
	  else if(opt_l < a && opt_r <= b){
		  return r_child->top2Q(x1, y1, x2, y2);
	  }
	  else if(a <= opt_l && b < opt_r){
		  return l_child->top2Q(x1, y1, x2, y2);
	  }
	  else{
		  return l_child->top2Q(x1, y1, x2, y2);
	  }
	  /**
	  else if(l_child){
		  cand = l_child->top2Q(x1, y1, x2, y2);
	  }
	  else if(r_child){
		  cand2 = r_child->top2Q(x1, y1, x2, y2);
	  }
	  else{
		  //manual search again?
	  }**/
  }
  else if( a < left_range && right_range < b){
	  return _top2Q();
  }
  else{
	  if(cand2) delete cand2;
	  return cand;   //return empty vector
  }

}

vector<int>* MyRDAG_node::_top2Q(){
 	  vector<int>* temp = new vector<int>();
	  int _x1, _y1, _x2, _y2;
	  if(opt_lx == 0){
		  if(opt_rx == 0){
			  if( (*(ref.list1))[opt_l] < (*(ref.list1))[opt_r]){
				  _x1 = 0;  _x2 = 0;
				  _y1 = opt_r;    _y2 = opt_l;
			  }
			  else{
				  _x1 = 0; _x2 = 0;
				  _y1 = opt_l;   _y2 = opt_r;
			  }
		  }
		  else{
			  if( (*(ref.list1))[opt_l] < (*(ref.list2))[opt_r]){
				  _x1 = 1; _x2 = 0;
				  _y1 = opt_r;  _y2 = opt_l;
			  }
			  else{
				  _x1 = 0;  _x2 = 1;
				  _y1 = opt_l;  _y2 = opt_r;
			  }
		  }
	  }
	  else{
		  if(opt_rx == 0){
			  if( (*(ref.list2))[opt_l] < (*(ref.list1))[opt_r]){
				  _x1 = 0;  _x2 = 1;
				  _y1 = opt_r;    _y2 = opt_l;
			  }
			  else{
				  _x1 = 1; _x2 = 0;
				  _y1 = opt_l;   _y2 = opt_r;
			  }
		  }
		  else{
			  if( (*(ref.list2))[opt_l] < (*(ref.list2))[opt_r]){
				  _x1 = 1;  _x2 = 1;
				  _y1 = opt_r;    _y2 = opt_l;
			  }
			  else{
				  _x1 = 1; _x2 = 1;
				  _y1 = opt_l;   _y2 = opt_r;
			  }
		  }
	  }

	  temp->push_back(_x1);        temp->push_back(_y1);
	  temp->push_back(_x2);        temp->push_back(_y2);

	  return temp;
}

bool MyRDAG_node::check_real_weight(){
	return check_real_weight_left() && check_real_weight_right();
}

bool MyRDAG_node::check_real_weight_left(){
  int pa_opt_l, pa_opt_r;
  if(!l_parent){
    return true;
  }//if end
  else{
	pa_opt_l = l_parent->opt_l;  pa_opt_r = l_parent->opt_r;
	if(pa_opt_l == pa_opt_r){
    	return true;
  	}
  	else{
		return false;
 	}
  }//else end
}
/*******************Member Function End ********************/

bool MyRDAG_node::check_real_weight_right(){
  int pa_opt_l, pa_opt_r;
  if(!r_parent){
	  return true;
  }
  else{
	  pa_opt_l = r_parent->opt_l;  pa_opt_r = r_parent->opt_r;
	  if(pa_opt_l == pa_opt_r){
	  	return true;
      }
  	  else{
      	return false;
      }
  }//else end

}//member function end
/*******************Member Function End ********************/

MyRDAG_node* MyRDAG_node::expect_duplicate(int l, int r){
	/**for(int i = 0; i < ref.dup_list->size(); i++){
		if( l == (*(ref.dup_list))[i]->get_left_range() 
			&& r == (*(ref.dup_list))[i]->get_right_range() ){
			return (*(ref.dup_list))[i];
		}
	}**/
	return (*ref.node_map)[pair<int, int>(l, r)];
}
/*******************Member Function End ********************/


void MyRDAG_node::get_range(int* left, int* right){
	*left = left_range;
	*right = right_range;
}





/**
 * Class : MyRDAG_node
 * Member Function Name: MyRDAG_node::get_opt_pos
 * param: int* x1, int* y1, int* x2, int* y2
 * return type: void
 **/
void MyRDAG_node::get_opt_pos(int* x1, int* y1, int* x2, int* y2){
    assert(ref.A1);
    assert(ref.A2);
	*x1 = this->opt_lx;  *y1 = this->opt_l;
	*x2 = this->opt_rx;  *y2 = this->opt_r;
}
/*********************************************************************************************/


void MyRDAG_node::get_opt_cols(){
    if( (!this->l_parent && !this->r_parent) ||
			(this->l_parent && (this->l_parent->opt_l == this->l_parent->opt_r)) ||
			(this->r_parent && (this->r_parent->opt_l == this->r_parent->opt_r))
			){
		vector<IndexPair> fr_vec, sr_vec;
        
		int _y11, _y12, _y21, _y22;
		_y11 = _y12 = _y21 = _y22 = this->left_range;
		for(int i = this->left_range; i <= this->right_range; i++){
			if( (*(ref.list1))[_y11] < (*(ref.list1))[i]){
				_y11 = i;
			}

			if( (*(ref.list2))[_y21] < (*(ref.list2))[i]){
				_y21 = i;
			}
		}//for loop end

		for(int i = this->left_range; i <= this->right_range; i++){
			if(_y11 == left_range && i == left_range && i < right_range){
				_y12 = left_range + 1;
				continue;
			}
			if( (*(ref.list1))[_y12] < (*(ref.list1))[i] &&
					(*(ref.list1))[i] < (*(ref.list1))[_y11] ){
				_y12 = i;
			}

			if(_y21 == left_range && i == left_range && i < right_range){
				_y22 = left_range + 1;
				continue;
			}

			if( (*(ref.list2))[_y22] < (*(ref.list2))[i] &&
					(*(ref.list2))[i] < (*(ref.list2))[_y21] ){
				_y22 = i;
			}
		}//for loop end
        fr_vec.push_back(IndexPair(_y11, (*(ref.list1))[_y11] ) );
		fr_vec.push_back(IndexPair(_y12, (*(ref.list1))[_y12] ) );
		sr_vec.push_back(IndexPair(_y21, (*(ref.list2))[_y21] ) );
		sr_vec.push_back(IndexPair(_y22, (*(ref.list2))[_y22] ) );

		int n1, n2;
        int x1, x2;
	    if(_y11 == _y12 && _y21 == _y22){
			if( fr_vec[0].elem > sr_vec[0].elem){
				n1 = (fr_vec)[0].index;
				x1 = 0;
				n2 = (sr_vec)[0].index;
				x2 = 1;
			}
			else{
				n1 = (sr_vec)[0].index;
				x1 = 1;
				n2 = (fr_vec)[0].index;
				x2 = 0;
			}
		}
		else{
			if( fr_vec[0].elem > sr_vec[0].elem ){
				n1 = (fr_vec)[0].index;
				x1 = 0;
				if( fr_vec[1].elem > sr_vec[0].elem ) {
					n2 = (fr_vec)[1].index;
					x2 = 0;
				}
				else{
					n2 = (sr_vec)[0].index;
					x2 = 1;
				}
			}
			else{
				n1 = (sr_vec)[0].index;
				x1 = 1;
				if( fr_vec[0].elem < sr_vec[1].elem ){
					n2 = (sr_vec)[1].index;
					x2 = 1;
				}
				else{
					n2 = (fr_vec)[0].index;
					x2 = 0;
				}
			}
		}

		if(n1 > n2){
			swap(n1, n2);
			swap(x1, x2);
		}

		this->opt_lx = x1; this->opt_l = n1;
		this->opt_rx = x2; this->opt_r = n2;
	}//if end
	else{
		int top1, top2, top2a;
		int top1_x, top2_x;
		if(this->l_parent){
			top1 = this->l_parent->opt_r;
			top1_x = this->l_parent->opt_rx;
			//cerr << "l_parent: " << l_parent->opt_l << ", " << l_parent->opt_r;
		}
		else if(this->r_parent){
			top1 = this->r_parent->opt_l;
			top1_x = this->r_parent->opt_lx;
			//cerr << "r_parent: " << r_parent->opt_l << ", " << r_parent->opt_r;
		}
        int top_val = (top1_x == 0) ? (*(this->ref.list1))[top1] : (*(this->ref.list2))[top1];
		top2a = top2 = this->left_range;
		bool isUp = true;
		for(int i = this->left_range; i <= this->right_range; i++){
			if(top1_x == 0 && top2 == top1){
				if(left_range != right_range){
					top2 += 1;
				}
				else{
					top2 = top1;
				}
			}
			if( (*(this->ref.list1))[top2] < (*(this->ref.list1))[i] &&
					(*(this->ref.list1))[i] < top_val){

				top2 = i;
			}
		}
		for(int j = this->left_range; j <= this->right_range; j++){
			if(top1_x == 1 && top2a == top1){
				if(left_range != right_range){
					top2a += 1;
					continue;
				}
				else{
					top2a = top1;
				}
			}

			if( (*(this->ref.list2))[top2a] < (*(this->ref.list2))[j] &&
					(*(this->ref.list2))[j] < top_val){

				top2a = j;
			}
		}
		if( top2a >= 0 && (*(this->ref.list1))[top2] < (*(this->ref.list2))[top2a]){
			top2_x = 1;
			top2 = top2a;
		}
		else{
			top2_x = 0;
		}
/**		vector<int>* topList = this->ref.list1;;
		if( (*(this->ref.list1))[top2] < (*(this->ref.list2))[top2] ){
			isUp = false; topList = this->ref.list2;
		}

		for(int i = this->left_range + 1; i < this->right_range ; i++){
			cout << "  opt_cols_process top2: " << top2  << endl;
			if((*(topList))[top2] < (*(this->ref.list1))[i] &&
					(*(this->ref.list1))[i] < top_val ){
				if( (*(this->ref.list1))[i] < (*(this->ref.list2))[i] ){
					isUp = false; topList = this->ref.list2;
				}
				else{
					isUp = true; topList = this->ref.list1;
				}
				top2 = i;
			}
			else if( (*(topList))[top2] < (*(this->ref.list2))[i] &&
					 (*(this->ref.list2))[i] < top_val ){
				isUp = false; topList = this->ref.list2;
				top2 = i;
			}
			else{
				continue;
			}
		}//for end
**/
		if(top1 < top2){
			this->opt_lx = top1_x;  this->opt_l = top1;
			this->opt_rx = top2_x;  this->opt_r = top2;
		}
		else{
			this->opt_lx = top2_x;  this->opt_l = top2;
			this->opt_rx = top1_x;  this->opt_r = top1;
		}
	}//else end
}
/**MyRDAG_node member function END**/

void MyRDAG_node::get_opt_cols(int* opt_left, int* opt_right){
	*opt_left = this->opt_l;
	*opt_right = this->opt_r;
}

int MyRDAG_node::get_left_range(){
	return left_range;
}

int MyRDAG_node::get_right_range(){
	return right_range;
}

bool MyRDAG_node::get_weight(){
	return weight ;
}

bool MyRDAG_node::get_weight1(){
	return weight_bit1;
}

bool MyRDAG_node::get_weight2(){
	return weight_bit2;
}

bool MyRDAG_node::get_leaf(){
	return leaf;
}

node_ref MyRDAG_node::get_ref(){
  return ref;
}

vector<int> MyRDAG_node::getTop2Int(int num){
  assert(num == 1 || num == 2);
  vector<int> tempVec;
  int best_idx, s_idx;
  int best, s_best;
  best = -1;  s_best = -2;
  s_idx = best_idx = -1;
  vector<int>* list_pt;

  if(num == 1){
	  list_pt = ref.list1;
  }
  else{
	  list_pt = ref.list2;
  }
  for(int i = this->left_range; i <= this->right_range; i++){

	  if( best < (*list_pt)[i] ){
		  best = (*list_pt)[i];
		  best_idx = i;
	  }
  }
  for(int j = this->left_range; j <= this->right_range; j++){

	  if( s_best < (*list_pt)[j] &&
			  (*list_pt)[j] < best ){
		  s_best = (*list_pt)[j];
		  s_idx = j;
	  }
  }
  tempVec.push_back(best_idx); 
  if(left_range == right_range){
	tempVec.push_back(best_idx);
  }
  else{
  	tempVec.push_back(s_idx);
  }
  return tempVec;
}

/**vector<int> MyRDAG_node::getTop2Int(int num){
	assert(num == 1 || num == 2);
	vector<IndexPair>* heap;
	if(num == 1){
		heap = &range_heap1;
	}
	else if(num == 2){
		heap = &range_heap2;
	}
	
	vector<int> tempVec;

	IndexPair r_temp;
	r_temp = heap->front();
	tempVec.push_back(heap->front().index );  
	std::pop_heap(heap->begin(), heap->end() ); heap->pop_back();
	tempVec.push_back(heap->front().index ); heap->push_back(r_temp);
	std::push_heap(heap->begin(), heap->end() );

	return tempVec;
}**/

MyRDAG_node* MyRDAG_node::get_left_parent(){
	return l_parent;
}

MyRDAG_node* MyRDAG_node::get_right_parent(){
    return r_parent;
}

MyRDAG_node* MyRDAG_node::get_left_child(){
	return l_child;
}

MyRDAG_node* MyRDAG_node::get_right_child(){
    return r_child;
}


void MyRDAG_node::set_left_range(int l){
	left_range = l;
}

void MyRDAG_node::set_right_range(int r){
	right_range = r;
}
void MyRDAG_node::set_weight(bool w){
	weight = w;
}

void MyRDAG_node::set_weight1(bool w){
	weight_bit1 = w;
}

void MyRDAG_node::set_weight2(bool w){
	weight_bit2 = w;
}

void MyRDAG_node::set_left_parent(MyRDAG_node* node){
	assert(l_parent == NULL && node != NULL);
	l_parent = node;
}

void MyRDAG_node::set_right_parent(MyRDAG_node* node){
	assert(r_parent == NULL && node != NULL);
	r_parent = node;
}

void MyRDAG_node::set_left_child(MyRDAG_node* node){
	assert(!l_child);
	l_child = node;
}

void MyRDAG_node::set_right_child(MyRDAG_node* node){
	assert(!r_child);
	r_child = node;
}
