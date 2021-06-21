#ifndef AVL_H
#define AVL_H

#include <memory>
#include <string>

#include "json.hpp"

class AVL;

class AVLNode {
 public:
 	AVLNode(int key);
 	AVLNode(int key, std::weak_ptr<AVLNode> parent);
 	bool IsLeaf() const;
 	bool IsMissingChild() const;
 	bool HasLeftChild() const;
 	bool HasRightChild() const;
 	void DeleteChild(std::shared_ptr<AVLNode> v);
 	void ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u);

 private:
  int height_;
  int bf_;
  int key_;
  std::weak_ptr<AVLNode> parent_;
  std::shared_ptr<AVLNode> left_;
  std::shared_ptr<AVLNode> right_;

  friend AVL;
}; // class AVLNode

class AVL {
 public:
 	AVL();

    int max(int a, int b);
 	void Insert(int key);
 	bool Delete(int key);
 	bool Find(int key) const;
 	nlohmann::json JSON() const;
 	size_t size() const;
 	bool empty() const;
 	int DeleteMin();
    std::shared_ptr<AVLNode> showRoot();
	std::shared_ptr<AVLNode> showParent(std::shared_ptr<AVLNode> node);
    void UpdateHeight(std::shared_ptr<AVLNode> lastNode);	
	void BalanceFactor(std::shared_ptr<AVLNode> node) const; 
    void PostOrderHeightRefresh();
	void HeightRefresh(std::shared_ptr<AVLNode> node);
	void BalanceFactorRefresh(std::shared_ptr<AVLNode> node);

	


 private:
	void DeleteLeaf(std::shared_ptr<AVLNode> currentNode);
	int DeleteMin(std::shared_ptr<AVLNode> currentNode);

	std::shared_ptr<AVLNode> InsertBalance(std::shared_ptr<AVLNode> child_node);
	std::shared_ptr<AVLNode> RRotation(std::shared_ptr<AVLNode> node);
    std::shared_ptr<AVLNode> LRotation(std::shared_ptr<AVLNode> node);

 	std::shared_ptr<AVLNode> root_;
 	size_t size_;
}; // class AVL


#endif