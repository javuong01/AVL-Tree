#include "AVL.h"

#include <cassert>
#include <iostream>
#include <string>
#include <queue>

#include "json.hpp"


AVLNode::AVLNode(int key) :
    height_(0),
	bf_(0),
	key_(key),
	parent_(std::weak_ptr<AVLNode>()),
	left_(nullptr),
	right_(nullptr) {}

AVLNode::AVLNode(int key, std::weak_ptr<AVLNode> parent) :
    height_(0),
	bf_(0),
	key_(key),
	parent_(parent),
	left_(nullptr),
	right_(nullptr) {}

bool AVLNode::IsLeaf() const {
	return left_ == nullptr && right_ == nullptr;
}

bool AVLNode::HasLeftChild() const {
	return left_ != nullptr;
}

bool AVLNode::HasRightChild() const {
	return right_ != nullptr;
}

void AVLNode::DeleteChild(std::shared_ptr<AVLNode> v) {
	if (left_ == v) {
		left_ = nullptr;
	} else if (right_ == v) {
		right_ = nullptr;
	} else {
		std::cerr << "AVLNode::DeleteChild Error: non-child passed as argument\n";
		exit(EXIT_FAILURE);
	}
}

void AVLNode::ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u) {
	if (left_ == u || right_ == u) {
		std::cerr << "AVLNode::ReplaceChild Error: child passed as replacement\n";
	}
	if (left_ == v) {
		left_ = u;
		u->parent_ = v->parent_;
	} else if (right_ == v) {
		right_ = u;
		u->parent_ = v->parent_;
	} else {
		std::cerr << "AVLNode::ReplaceChild Error: non-child passed as argument\n";
		exit(EXIT_FAILURE);
	}
}






AVL::AVL() : root_(nullptr), size_(0) {}

std::shared_ptr<AVLNode> AVL::showRoot() {
	return root_;
}

int AVL::max(int a, int b) {
	return (a > b)? a : b;
}

void AVL::UpdateHeight(std::shared_ptr<AVLNode> lastNode) {
    	if (lastNode->left_ == nullptr && lastNode->right_ != nullptr) {    //updating height   might be unecessary here
			lastNode->height_ = 1 + max(lastNode->right_->height_, 0);
		}
		else if (lastNode->left_ != nullptr && lastNode->right_ == nullptr) {
			lastNode->height_ = 1 + max(lastNode->left_->height_, 0);
		}
		else if (lastNode->IsLeaf()){
			lastNode->height_ = 0;
		}
		else {
		lastNode->height_ = 1 + max(lastNode->left_->height_, lastNode->right_->height_);
		}
}

void AVL::BalanceFactor(std::shared_ptr<AVLNode> node) const{

	if (node->left_ != nullptr && node->right_ != nullptr) {       //All heights are off by one due to prof's solutions
		node->bf_ = ((node->right_->height_ + 1) - (node->left_->height_ + 1));
	}
	else if (node->left_ != nullptr && node->right_ == nullptr) {
		node->bf_ = (0 - (node->left_->height_+ 1));     //all nodes have height increased by 1 temporarily
	}
	else if (node->left_ == nullptr && node->right_ != nullptr) {
		node->bf_ = (node->right_->height_+ 1);            //All nodes have height increased by 1 temporarily
	}
	else if (node->left_ == nullptr && node->right_ == nullptr) {
		node->bf_ = 0;
	}
	else{
		return;
	}
}

void AVL::PostOrderHeightRefresh() {
	HeightRefresh(showRoot());
	BalanceFactorRefresh(showRoot());
}

void AVL::HeightRefresh(std::shared_ptr<AVLNode> node) {
	if (node == nullptr){
		return;
	}
	HeightRefresh(node->left_);
	HeightRefresh(node->right_);
	UpdateHeight(node);
}

void AVL::BalanceFactorRefresh(std::shared_ptr<AVLNode> node) {
	if (node == nullptr){
		return;
	}
	BalanceFactorRefresh(node->left_);
	BalanceFactorRefresh(node->right_);
	BalanceFactor(node);
}

void AVL::Insert(int key) {
	if (root_ == nullptr) {
		root_ = std::make_shared<AVLNode>(key);
		size_++;
		return;
	}
	std::shared_ptr<AVLNode> currentNode = root_, lastNode = nullptr;
	while (currentNode != nullptr) {
		lastNode = currentNode;
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	if (key < lastNode->key_) {
		lastNode->left_ = std::make_shared<AVLNode>(key, lastNode);
	} else {
		lastNode->right_ = std::make_shared<AVLNode>(key, lastNode);
	}
	PostOrderHeightRefresh();
	InsertBalance(lastNode);	
	PostOrderHeightRefresh();
	size_++;
}

bool AVL::Delete(int key) {
	std::shared_ptr<AVLNode> currentNode = root_;
	while (currentNode != nullptr) {
		if (currentNode->key_ == key) {
			if (currentNode->IsLeaf()) {
				DeleteLeaf(currentNode);
			} else if (currentNode->left_ == nullptr) {
				assert(currentNode->right_ != nullptr);
				std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				parent->ReplaceChild(currentNode, currentNode->right_);
				size_--; assert(size_ >= 0);
			} else if (currentNode->right_ == nullptr) {
				assert(currentNode->left_ != nullptr);
				std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				parent->ReplaceChild(currentNode, currentNode->left_);
				size_--; assert(size_ >= 0);
			} else {
				currentNode->key_ = DeleteMin(currentNode);
			}
		}
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	return false;
}

int AVL::DeleteMin() {
	return DeleteMin(root_);
}


void AVL::DeleteLeaf(std::shared_ptr<AVLNode> currentNode) {
	std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
	if (parent == nullptr) {
		// Delete root
		root_ = nullptr;
		size_--; assert(size_ == 0);
	} else {
		if (parent->right_ == currentNode) {
			parent->right_ = nullptr;
		} else if (parent->left_ == currentNode) {
			parent->left_ = nullptr;
		} else {
			std::cerr << "AVL::DeleteLeaf Error: inconsistent state\n";
		}
		size_--; assert(size_ >= 0);
	}
}

int AVL::DeleteMin(std::shared_ptr<AVLNode> currentNode) {
	std::shared_ptr<AVLNode> lastNode = nullptr;
	while (currentNode != nullptr) {
		lastNode = currentNode;
		currentNode = currentNode->left_;
	}
	int result = lastNode->key_;
	std::shared_ptr<AVLNode> parent = lastNode->parent_.lock();
	if (parent == nullptr) {
		// lastNode is root
		if (lastNode->right_ != nullptr) {
			root_ = lastNode->right_;
			lastNode->right_->parent_.reset();
		} else {
			root_ = nullptr;
		}
	} else {
		// lastNode under the root
		if (lastNode->right_ != nullptr) {
			parent->left_ = lastNode->right_;
			lastNode->right_->parent_ = parent;
		} else {
			parent->left_ = nullptr;
		}
  }
	size_--; assert(size_ >= 0);
	return result;
}

size_t AVL::size() const {
	return size_;
}

bool AVL::empty() const {
	return size_ == 0;
}

bool AVL::Find(int key) const {
	std::shared_ptr<AVLNode> currentNode = root_;
	while (currentNode != nullptr) {
		if (currentNode->key_ == key) {
			return true;
		}
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	return false;
}

nlohmann::json AVL::JSON() const {     //std::string 
	nlohmann::json result;
	std::queue< std::shared_ptr<AVLNode> > nodes;
	if (root_ != nullptr) {
		result["root"] = root_->key_;
		nodes.push(root_);
		while (!nodes.empty()) {
			auto v = nodes.front();
			nodes.pop();
			std::string key = std::to_string(v->key_);
			result[key]["height"] = v->height_;
			result[key]["balance factor"] = v->bf_;
			if (v->left_ != nullptr) {
				result[key]["left"] = v->left_->key_;
				nodes.push(v->left_);
			}
			if (v->right_ != nullptr) {
				result[key]["right"] = v->right_->key_;
				nodes.push(v->right_);
			}
			if (v->parent_.lock() != nullptr) {
				result[key]["parent"] = v->parent_.lock()->key_;
			} else {
				result[key]["root"] = true;
			}
		}
	}
	result["height"] = root_->height_;
	result["size"] = size_;
	return result; //result.dump(2) + "\n";
}


//Rotation Zone

std::shared_ptr<AVLNode> AVL::RRotation(std::shared_ptr<AVLNode> node) {    //y is a subtree of x
    std::shared_ptr<AVLNode> x = node->left_;   //looks like this        node
    std::shared_ptr<AVLNode> y = x->right_;     //                   x        
                                                //                        y
    //rotation
	if (node->parent_.lock() != nullptr){
		x->parent_ = node->parent_.lock();  //might need to move this line above the x->right
		if (node->parent_.lock()->left_ == node){
			x->parent_.lock()->left_ = x;
		}
		if (node->parent_.lock()->right_ == node){
			x->parent_.lock()->right_ = x;
		}	
	}
	else {
		auto temp = node->parent_;
		x->parent_ = temp;
		root_ = x;
	}
    x->right_ = node;     
    node->parent_ = x;        
    if (y != nullptr){
		node->left_ = y;
		y->parent_ = node;  
	}
	else {
		node->left_ = nullptr;
	}

                                                //looks like this        x
                                                //                           node
                                                //                       y  
    
    //Updating heights
	PostOrderHeightRefresh();

    //returning the new root in order to continue rotations
    return x;
}

std::shared_ptr<AVLNode> AVL::LRotation(std::shared_ptr<AVLNode> node) {  //y is a subtree of x
    std::shared_ptr<AVLNode> x = node->right_;    //looks like this        node
    std::shared_ptr<AVLNode> y = x->left_;        //                             x
                                                  //                         y
    //rotation
	if (node->parent_.lock() != nullptr) {
    	x->parent_ = node->parent_.lock();            //might need to move this line above the x->left
		if (node->parent_.lock()->left_ == node){
			x->parent_.lock()->left_ = x;
		}
		if (node->parent_.lock()->right_ == node){
			x->parent_.lock()->right_ = x;
		}	
    }
	else {
		auto temp = node->parent_;
		x->parent_ = temp;
		root_ = x;
	}
	x->left_ = node;  
    node->parent_ = x;        
    if (y != nullptr){
		node->right_ = y;
		y->parent_ = node;  
	}
	else {
		node->right_ = nullptr;
	}                                        
                                                  
                                                //looks like this          x
                                                //                  node
                                                //                         y
   
    //Updating heights
	PostOrderHeightRefresh();

    //returning the new root in order to continue rotations
    return x;
}

std::shared_ptr<AVLNode> AVL::InsertBalance(std::shared_ptr<AVLNode> child_node) { //might need to edit bc it might be wrong. maybe more rotations in each double rotation thing?
	
	if (child_node->parent_.lock() != nullptr){
		std::shared_ptr<AVLNode> parent_node = child_node->parent_.lock();

		//LL
		if (parent_node->bf_ < -1 && child_node->bf_ < 0) {
			child_node = RRotation(parent_node);
			// child_node = InsertBalance(child_node);
			if (child_node->parent_.lock() != nullptr){
				parent_node = child_node->parent_.lock();
				return parent_node;
			}
			else {
				return child_node;
			}
		}

		//LR
		if (parent_node->bf_ < -1 && child_node->bf_ > 0) {
			child_node = LRotation(child_node);
			child_node = RRotation(parent_node);
			// child_node = InsertBalance(child_node);
			if (child_node->parent_.lock() != nullptr){
				parent_node = child_node->parent_.lock();
				return parent_node;
			}
			else {
				return child_node;
			}
		}
		
		//RR            
		if (parent_node->bf_ > 1 && child_node->bf_ > 0) {
			child_node = LRotation(parent_node);       //if statements for all scenarios
			// child_node = InsertBalance(child_node);
			if (child_node->parent_.lock() != nullptr){
				parent_node = child_node->parent_.lock();
				return parent_node;
			}
			else {
				return child_node;
			}
		}
	
		//RL           
		if (parent_node->bf_ > 1 && child_node->bf_ < 0) {
			child_node = RRotation(child_node);
			child_node = LRotation(parent_node);
			// child_node = InsertBalance(child_node);
			if (child_node->parent_.lock() != nullptr){
				parent_node = child_node->parent_.lock();
				return parent_node;
			}
			else {
				return child_node;
			}
		}

		return InsertBalance(parent_node);
		// return parent_node;

	}
	else {
		return child_node;
	}
 
    //returning the unchanged pointer if no rotations happened


    return child_node;
}