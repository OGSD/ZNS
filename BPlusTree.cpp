#include <bits/stdc++.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <utility>
#include <vector>

using namespace std;

//new revise part 
void zone_choose(int& zoneaddr, long long int& zonesize, long long int wsize) {
  if(zonesize - wsize < 0) {
    zoneaddr = zoneaddr + 0x4000;
    zonesize = 67108864 - wsize;
  }
  else {
    zonesize = zonesize - wsize;
  }
}

//new revise part
long long int data_transform(int count, long long int data) {
  if(count == 1 || count ==2) {
    data = data * 512;
  }
  return data;
}

//new revise part
long long int write_size(long long size) {
  long long int write_size;
  long long int multiple = size / 4096;
  if(size % 4096 == 0) {
      write_size = multiple * 4096;
  }
  else {
    write_size = (multiple + 1) * 4096;
  }
  return write_size;
}

//new rvise part
string write_command(int zone_addr, long long int wsize) {
  string temp_result;
  string final_temp_result;
  string write_str = "echo \"aaa\" | sudo nvme zns zone-append /dev/nvme0n1 -s ";
  string write_temp = " -z ";
  string f_write_str; 
  f_write_str = write_str + to_string(zone_addr) + write_temp + to_string(wsize);

  //cout << "write_command : " << f_write_str << endl;

  const char* wcommand = f_write_str.c_str();

  FILE* fp = NULL;
  char buf[1024], result[4096], temp[4096];
  if((fp = popen(wcommand, "r")) != NULL) {
    while(fgets(buf, 1024, fp) != NULL) {
      strcat(result, buf);
    }
    pclose(fp);
    fp == NULL;
  }

  temp_result = result;
  final_temp_result = temp_result.substr(temp_result.find("A")+2, -1);
  strcpy(temp, final_temp_result.data());

  //int lenght = (sizeof(result) / sizeof(char)) -30;
  //strncpy(temp, result + 29, lenght);
  memset(result, 0, sizeof(result));
  cout << "result : " << temp << endl;
  return temp;
}

//revise part
class twoD_value {
  public :
    long long int size;
    char zLBA[4096];
};

class Node {
 public:
  Node(Node *parent = nullptr, bool isLeaf = false, Node *prev_ = nullptr,
       Node *next_ = nullptr)
      : parent(parent), isLeaf(isLeaf), prev(prev_), next(next_) {
    if (next_) {
      next_->prev = this;
    }

    if (prev_) {
      prev_->next = this;
    }
  }

  //revise part
  vector<twoD_value> two_values;

  vector<long long int> keys;
  Node *parent;

  vector<Node *> children;
  //vector<int> values;
  Node *next;
  Node *prev;

  bool isLeaf;
  
  //revise part
  long long int key_place(Node* prev, long long int key) {
    for (int i = 0; i < keys.size(); i++) {
      if (key < keys[i]) {
        if (i == 0) {
          return prev->keys[keys.size()-1];
        }
        return keys[i-1];
      }
    }
    return keys[keys.size()-1];
  }

  //revise part
  long long int next_key_place(Node* node, long long int key) {
    int i;
    for (i = 0; i < keys.size(); i++) {
      if (key == keys[i]){
        if (i == keys.size()) {
          return next->keys[0];
        }
      }
    }
    return keys[i+1];
  }

  int indexOfChild(long long int key) {
    for (int i = 0; i < keys.size(); i++) {
      if (key < keys[i]) {
        return i;
      }
    }
    return keys.size();
  }

  int indexOfKey(long long int key) {
    for (int i = 0; i < keys.size(); i++) {
      if (key == keys[i]) {
        return i;
      }
    }
    return -1;
  }

  //revise part
  vector<int> keys_arr (){
    vector<int> temp{};
    for (int i = 0; i < keys.size(); i++){
      int j = keys[i];
      temp.push_back(j);
    }

    return temp;
  } 

  //revise part
  Node *findChild(long long int key) {
    int i = indexOfChild(key);
    if (i == 0) return children[i];
    else return NULL;
  } 

  Node *getChild(long long int key) { return children[indexOfChild(key)]; }

  void setChild(long long int key, vector<Node *> value) {
    int i = indexOfChild(key);
    keys.insert(keys.begin() + i, key);
    children.erase(children.begin() + i);
    children.insert(children.begin() + i, value.begin(), value.end());
  }

  tuple<long long int, Node *, Node *> splitInternal() {
    Node *left = new Node(parent, false, nullptr, nullptr);
    int mid = keys.size() / 2;

    copy(keys.begin(), keys.begin() + mid, back_inserter(left->keys));
    copy(children.begin(), children.begin() + mid + 1,
         back_inserter(left->children));

    for (Node *child : left->children) {
      child->parent = left;
    }

    long long int key = keys[mid];
    keys.erase(keys.begin(), keys.begin() + mid + 1);
    children.erase(children.begin(), children.begin() + mid + 1);

    return make_tuple(key, left, this);
  }

  //revise part
  twoD_value get(long long int key) {
    int index = -1;
    for (int i = 0; i < keys.size(); ++i) {
      if (keys[i] == key) {
        index = i;
        break;
      }
    }

    if (index == -1) {
      cout << "key " << key << " not found" << endl;
    }

    //revise part
    return two_values[index];
  }

  void set(long long int key, twoD_value value) {
    int i = indexOfChild(key);
    if (find(keys.begin(), keys.end(), key) == keys.end()) {
      keys.insert(keys.begin() + i, key);
      two_values.insert(two_values.begin() + i, value);
    } else {
      two_values[i - 1] = value;
    }
  }

  tuple<long long int, Node *, Node *> splitLeaf() {
    Node *left = new Node(parent, true, prev, this);
    int mid = keys.size() / 2;

    left->keys = vector<long long int>(keys.begin(), keys.begin() + mid);
    left->two_values = vector<twoD_value>(two_values.begin(), two_values.begin() + mid);

    keys.erase(keys.begin(), keys.begin() + mid);
    two_values.erase(two_values.begin(), two_values.begin() + mid);

    return make_tuple(keys[0], left, this);
  }
};

class BPlusTree {
 public:
  BPlusTree(int _maxCapacity = 4) {
    root = new Node(nullptr, true, nullptr, nullptr);
    maxCapacity = _maxCapacity > 2 ? _maxCapacity : 2;
    minCapacity = maxCapacity / 2;
    depth = 0;
  }

  Node *root;
  int maxCapacity;
  int minCapacity;
  int depth;

  Node *findLeaf(long long int key) {
    Node *node = root;
    while (!node->isLeaf) {
      node = node->getChild(key);
    }
    return node;
  }

  twoD_value get(long long int key) { return findLeaf(key)->get(key); }

  //new revise part
  bool tree_empty() {
    Node* node = root;
    if(root->keys.empty()) return 1;
    else return 0;
  }

  // new revise part
  long long int insert_pointer(long long int key) {
    Node* node = root;
    Node* temp;
    int i = 0;
    while(!node->isLeaf) {
      node = node->children[0];  
    }
    for(i = 0; i < node->keys.size(); i++) {
      if(key < node->keys[i]) {
        if(i == 0) {
          temp = node;
          node = node->prev;

          if(node == NULL) {
            node = temp;
            return node->keys[0];
          }

          else {
            return node->keys[node->keys.size()-1];
          }
        }

        else {
          return node->keys[i-1];
        }
      }

      if(i == node->keys.size()-1) {
        temp = node;
        node = node->next;

        if(node != NULL) {
          i = -1;
        }

        else {
          node = temp;
          break;
        }
      }
    }
    return node->keys[node->keys.size()-1];
  }

  //new revise part
  int overlapping(long long int pointer_key, long long int key, twoD_value value) {
    twoD_value temp;
    temp = get(pointer_key);
    //cout << "size : " << temp.size << endl;
    //cout <<  "zlba : " << temp.zLBA << endl;
    long long int pointer_size;
    pointer_size = temp.size;

    long long int size;
    size = value.size;
    //cout << "value_size : " << size << endl;

    if(key < pointer_key && key+size-1 >= pointer_key) {
      //cout << "type : 1" << endl;
      return 1;
    }

    else if(key == pointer_key) {
      //cout << "type : 2" << endl;
      return 2;
    }

    else if(key > pointer_key) {
      if(key > pointer_key+pointer_size-1) {
        if(last_key(pointer_key)) {
          //cout << "type : 0" << endl;
          return 0;
        }
        else {
          int type;
          long long int next_pointer;
          next_pointer = get_next_pointer(pointer_key);
          type = overlapping(next_pointer, key, value);
          //cout << "type : " << type << endl;
          return type;
        }
      }
      else {
        //cout << "type : 3" << endl;
        return 3;
      }
    }
    return 0;
  }

  //new revise part
  void final_insert(int type, long long int pointer_key, long long int key, twoD_value value) {
    twoD_value pointer;
    pointer = get(pointer_key);
    long long int pointer_size;
    pointer_size = pointer.size;

    long long int size;
    size = value.size;

    switch (type) {
      case 0 :
        //cout << "case : 0" << endl;
        set(key, value);
        //cout << "finish" << endl;
        break;
    
      case 1 :
        //cout << "case : 1" << endl;
        if(key+size-1 < pointer_key+pointer_size-1) {
          long long int modify_key;
          twoD_value modify;

          modify_key = key+size;
          //cout << "check : " << modify_key << endl;;
          modify.size = pointer_size-(modify_key-key);
          strcpy(modify.zLBA, pointer.zLBA);

          remove(pointer_key);
          set(modify_key, modify);
          set(key, value);
        }
        else {
          if(last_key(pointer_key)) {
            remove(pointer_key);
            set(key, value);
          }
          
          else {
            int type1;
            long long int next_pointer;
            next_pointer = get_next_pointer(pointer_key);
            remove(pointer_key);
            type1 = overlapping(next_pointer, key, value);
            final_insert(type1, next_pointer, key, value);
          }

        }
        //cout << "finish" << endl;
        break;

      case 2 :
        //cout << "case : 2" << endl;
        if(key+size-1 < pointer_key+pointer_size-1) {
          long long int modify_key;
          twoD_value modify;

          modify_key = key+size;
          modify.size = pointer_size-(modify_key-key);
          strcpy(modify.zLBA, pointer.zLBA);

          remove(pointer_key);
          set(modify_key, modify);
          set(key, value);
        }

        else {
          if(last_key(pointer_key)) {
            remove(pointer_key);
            set(key, value);
          }

          else {
            int type1;
            long long int next_pointer;
            next_pointer = get_next_pointer(pointer_key);
            remove(pointer_key);
            type1 = overlapping(next_pointer, key, value);
            final_insert(type1, next_pointer, key, value);
          }
        }
        //cout << "finish" << endl;
        break;
    
      case 3 :
        //cout << "case : 3" << endl;
        if(key+size-1 < pointer_key+pointer_size-1) {
          long long modify_key, modify_key1;
          twoD_value modify, modify1;

          modify_key = pointer_key;
          modify.size = key-modify_key;
          strcpy(modify.zLBA, pointer.zLBA);

          modify_key1 = key+size;
          modify1.size = pointer_key+pointer_size-1-modify_key1+1;
          strcpy(modify1.zLBA, pointer.zLBA);

          remove(pointer_key);
          set(modify_key, modify);
          set(modify_key1, modify1);
          set(key, value);
        }

        else if(key+size-1 == pointer_key+pointer_size-1) {
          long long int modify_key;
          twoD_value modify;

          modify_key = pointer_key;
          modify.size = size-pointer_size;
          strcpy(modify.zLBA, pointer.zLBA);

          remove(pointer_key);
          set(modify_key, modify);
          set(key, value);
        }
        else if(key+size-1 > pointer_key+pointer_size-1) {
          if(last_key(pointer_key)) {
            long long int modify_key;
            twoD_value modify;

            modify_key = pointer_key;
            modify.size = size-pointer_size;
            strcpy(modify.zLBA, pointer.zLBA);

            remove(pointer_key);
            set(modify_key, modify);
            set(key, value);
          }

          else {
            int type1;
            long long int modify_key;
            twoD_value modify;

            modify_key = pointer_key;
            modify.size = size-pointer_size;
            strcpy(modify.zLBA, pointer.zLBA);

            long long int next_pointer;
            next_pointer = get_next_pointer(pointer_key);

            remove(pointer_key);
            set(modify_key, modify);

            type1 = overlapping(next_pointer, key, value);
            final_insert(type1, next_pointer, key, value);
          }
        }
        //cout << "finish" << endl;
        break;

      /*  
      default:
        break;
      */
    }
  }
  
  //new revise part
  bool last_key(long long int key){
    Node* node = root;
    Node* temp;
    while(!node->isLeaf) {
      node = node->children[0];
    }
    for(int i = 0; i < node->keys.size(); i++) {
      if(i == node->keys.size()-1) {
        temp = node;
        node = node->next;
        if(node != NULL) {
          i = -1;
        }
        else {
          node = temp;
          break;
        }
      }
    }
    if(key == node->keys[node->keys.size()-1]) {
      return 1;
    }
    return 0;
  }

  //new revise part
  long long int get_next_pointer(long long int key) {
    Node* node = root;
    while (!node->isLeaf) {
      node = node->getChild(key);
    }
    for (int i = 0; i < node->keys.size(); i++) {
      if (key == node->keys[i] && i != (node->keys.size())-1) {
        cout << "next_key_place : " << node->keys[i+1] << endl;
        return node->keys[i+1];
      }
      else if (key == node->keys[i] && i == (node->keys.size())-1) {
        node = node->next;
        if (node != NULL) {
          break;
        }
      }
    }
    cout << "next_key_place : " << node->keys[0] << endl;
    return node->keys[0];
  }

  void set(long long int key, twoD_value value) {
    Node *leaf = findLeaf(key);
    leaf->set(key, value);
    if (leaf->keys.size() > maxCapacity) {
      insert(leaf->splitLeaf());
    }
  }

  void insert(tuple<long long int, Node *, Node *> result) {
    long long int key = std::get<0>(result);
    Node *left = std::get<1>(result);
    Node *right = std::get<2>(result);
    Node *parent = right->parent;
    if (parent == nullptr) {
      left->parent = right->parent = root =
          new Node(nullptr, false, nullptr, nullptr);
      depth += 1;
      root->keys = {key};
      root->children = {left, right};
      return;
    }
    parent->setChild(key, {left, right});
    if (parent->keys.size() > maxCapacity) {
      insert(parent->splitInternal());
    }
  }

  void removeFromLeaf(long long int key, Node *node) {
    int index = node->indexOfKey(key);
    if (index == -1) {
      cout << "Key " << key << " not found! Exiting ..." << endl;
      exit(0);
    }
    node->keys.erase(node->keys.begin() + index);
    node->two_values.erase(node->two_values.begin() + index);
    if (node->parent) {
      int indexInParent = node->parent->indexOfChild(key);
      if (indexInParent)
        node->parent->keys[indexInParent - 1] = node->keys.front();
    }
  }

  void removeFromInternal(long long int key, Node *node) {
    int index = node->indexOfKey(key);
    if (index != -1) {
      Node *leftMostLeaf = node->children[index + 1];
      while (!leftMostLeaf->isLeaf)
        leftMostLeaf = leftMostLeaf->children.front();

      node->keys[index] = leftMostLeaf->keys.front();
    }
  }

  void borrowKeyFromRightLeaf(Node *node, Node *next) {
    node->keys.push_back(next->keys.front());
    next->keys.erase(next->keys.begin());
    node->two_values.push_back(next->two_values.front());
    next->two_values.erase(next->two_values.begin());
    for (int i = 0; i < node->parent->children.size(); i++) {
      if (node->parent->children[i] == next) {
        node->parent->keys[i - 1] = next->keys.front();
        break;
      }
    }
  }

  void borrowKeyFromLeftLeaf(Node *node, Node *prev) {
    node->keys.insert(node->keys.begin(), prev->keys.back());
    prev->keys.erase(prev->keys.end() - 1);
    node->two_values.insert(node->two_values.begin(), prev->two_values.back());
    prev->two_values.erase(prev->two_values.end() - 1);
    for (int i = 0; i < node->parent->children.size(); i++) {
      if (node->parent->children[i] == node) {
        node->parent->keys[i - 1] = node->keys.front();
        break;
      }
    }
  }

  void mergeNodeWithRightLeaf(Node *node, Node *next) {
    node->keys.insert(node->keys.end(), next->keys.begin(), next->keys.end());
    node->two_values.insert(node->two_values.end(), next->two_values.begin(),
                        next->two_values.end());
    node->next = next->next;
    if (node->next) node->next->prev = node;
    for (int i = 0; i < next->parent->children.size(); i++) {
      if (node->parent->children[i] == next) {
        node->parent->keys.erase(node->parent->keys.begin() + i - 1);
        node->parent->children.erase(node->parent->children.begin() + i);

        break;
      }
    }
  }

  void mergeNodeWithLeftLeaf(Node *node, Node *prev) {
    prev->keys.insert(prev->keys.end(), node->keys.begin(), node->keys.end());
    prev->two_values.insert(prev->two_values.end(), node->two_values.begin(),
                        node->two_values.end());

    prev->next = node->next;
    if (prev->next) prev->next->prev = prev;

    for (int i = 0; i < node->parent->children.size(); i++) {
      if (node->parent->children[i] == node) {
        node->parent->keys.erase(node->parent->keys.begin() + i - 1);
        node->parent->children.erase(node->parent->children.begin() + i);
        break;
      }
    }
  }

  void borrowKeyFromRightInternal(int myPositionInParent, Node *node,
                                  Node *next) {
    node->keys.insert(node->keys.end(), node->parent->keys[myPositionInParent]);
    node->parent->keys[myPositionInParent] = next->keys.front();
    next->keys.erase(next->keys.begin());
    node->children.insert(node->children.end(), next->children.front());
    next->children.erase(next->children.begin());
    node->children.back()->parent = node;
  }

  void borrowKeyFromLeftInternal(int myPositionInParent, Node *node,
                                 Node *prev) {
    node->keys.insert(node->keys.begin(),
                      node->parent->keys[myPositionInParent - 1]);
    node->parent->keys[myPositionInParent - 1] = prev->keys.back();
    prev->keys.erase(prev->keys.end() - 1);
    node->children.insert(node->children.begin(), prev->children.back());
    prev->children.erase(prev->children.end() - 1);
    node->children.front()->parent = node;
  }

  void mergeNodeWithRightInternal(int myPositionInParent, Node *node,
                                  Node *next) {
    node->keys.insert(node->keys.end(), node->parent->keys[myPositionInParent]);
    node->parent->keys.erase(node->parent->keys.begin() + myPositionInParent);
    node->parent->children.erase(node->parent->children.begin() +
                                 myPositionInParent + 1);
    node->keys.insert(node->keys.end(), next->keys.begin(), next->keys.end());
    node->children.insert(node->children.end(), next->children.begin(),
                          next->children.end());
    for (Node *child : node->children) {
      child->parent = node;
    }
  }

  void mergeNodeWithLeftInternal(int myPositionInParent, Node *node,
                                 Node *prev) {
    prev->keys.insert(prev->keys.end(),
                      node->parent->keys[myPositionInParent - 1]);
    node->parent->keys.erase(node->parent->keys.begin() + myPositionInParent -
                             1);
    node->parent->children.erase(node->parent->children.begin() +
                                 myPositionInParent);
    prev->keys.insert(prev->keys.end(), node->keys.begin(), node->keys.end());
    prev->children.insert(prev->children.end(), node->children.begin(),
                          node->children.end());
    for (Node *child : prev->children) {
      child->parent = prev;
    }
  }

  void remove(long long int key, Node *node = nullptr) {
    if (node == nullptr) {
      node = findLeaf(key);
    }
    if (node->isLeaf) {
      removeFromLeaf(key, node);
    } else {
      removeFromInternal(key, node);
    }

    if (node->keys.size() < minCapacity) {
      if (node == root) {
        if (root->keys.empty() && !root->children.empty()) {
          root = root->children[0];
          root->parent = nullptr;
          depth -= 1;
        }
        return;
      }

      else if (node->isLeaf) {
        Node *next = node->next;
        Node *prev = node->prev;

        if (next && next->parent == node->parent &&
            next->keys.size() > minCapacity) {
          borrowKeyFromRightLeaf(node, next);
        } else if (prev && prev->parent == node->parent &&
                   prev->keys.size() > minCapacity) {
          borrowKeyFromLeftLeaf(node, prev);
        } else if (next && next->parent == node->parent &&
                   next->keys.size() <= minCapacity) {
          mergeNodeWithRightLeaf(node, next);
        } else if (prev && prev->parent == node->parent &&
                   prev->keys.size() <= minCapacity) {
          mergeNodeWithLeftLeaf(node, prev);
        }
      } else {
        int myPositionInParent = -1;

        for (int i = 0; i < node->parent->children.size(); i++) {
          if (node->parent->children[i] == node) {
            myPositionInParent = i;
            break;
          }
        }

        Node *next;
        Node *prev;

        if (node->parent->children.size() > myPositionInParent + 1) {
          next = node->parent->children[myPositionInParent + 1];
        } else {
          next = nullptr;
        }

        if (myPositionInParent) {
          prev = node->parent->children[myPositionInParent - 1];
        } else {
          prev = nullptr;
        }

        if (next && next->parent == node->parent &&
            next->keys.size() > minCapacity) {
          borrowKeyFromRightInternal(myPositionInParent, node, next);
        }

        else if (prev && prev->parent == node->parent &&
                 prev->keys.size() > minCapacity) {
          borrowKeyFromLeftInternal(myPositionInParent, node, prev);
        }

        else if (next && next->parent == node->parent &&
                 next->keys.size() <= minCapacity) {
          mergeNodeWithRightInternal(myPositionInParent, node, next);
        }

        else if (prev && prev->parent == node->parent &&
                 prev->keys.size() <= minCapacity) {
          mergeNodeWithLeftInternal(myPositionInParent, node, prev);
        }
      }
    }
    if (node->parent) {
      remove(key, node->parent);
    }
  }

  void print(Node *node = nullptr, string _prefix = "", bool _last = true) {
    if (!node) node = root;
    cout << _prefix << "├ [";
    for (int i = 0; i < node->keys.size(); i++) {
      cout << node->keys[i];
      if (i != node->keys.size() - 1) {
        cout << ", ";
      }
    }
    cout << "]" << endl;

    _prefix += _last ? "   " : "╎  ";

    if (!node->isLeaf) {
      for (int i = 0; i < node->children.size(); i++) {
        bool _last = (i == node->children.size() - 1);
        print(node->children[i], _prefix, _last);
      }
    }
  }
};

int main() {
  BPlusTree tree(2);

  int zone_addr = 0x0;
  long long int zone_size = 67108864;

  int write_count = 0;
  
  string line, data;

  vector<long long int> final_data;

  ifstream row_data("24832 volumn ID.txt");
  
  while(getline(row_data, line)) {
    istringstream ss(line);
    int count = 0;

    while(ss >> data) {
      long long int temp = stoi(data);
      temp = data_transform(count, temp);
      count += 1;
      final_data.push_back(temp);
    }

    twoD_value TwoD_value;

    if(final_data[3] == 1) {
      const char* wcommand;
      write_count += 1;
      long long int wsize = write_size(final_data[2]);
      zone_choose(zone_addr, zone_size, wsize);
      
      if(zone_addr != -1) {
        TwoD_value.size = final_data[2];
        strcpy(TwoD_value.zLBA, write_command(zone_addr, wsize).c_str());

        //cout << "size : " << dec << TwoD_value.size << endl;
        //cout << "zlba : " << hex << TwoD_value.zLBA << endl;

        //tree is empty
        if(tree.tree_empty()) {
          tree.set(final_data[1], TwoD_value);
        }

        //tree is not empty
        else {
          long long int pointer;
          int type;
          pointer = tree.insert_pointer(final_data[1]);
          cout << "pointer: " << dec << pointer << endl;
          type = tree.overlapping(pointer, final_data[1], TwoD_value);
          tree.final_insert(type, pointer, final_data[1], TwoD_value);
        }
      }
      
      else {
        cout << "write_count" << write_count << endl;
        cout << "zone full" << endl;
        break;
      }

    }

    else {

    }
    final_data.clear();
  }

  tree.print();
  /*
  vector<int> random_list = {5,  8,  10,  15,   16,   19,  20};

  vector<int> random_list_2 =
      vector<int>(random_list.begin(), random_list.end());

  for (int r : random_list_2) {
    for (int i : random_list) {
      cout << endl << "-------------" << endl;
      cout << "Inserting " << i << endl << endl;
      cout << "-------------" << endl << endl;

      tree.set(i, i);
      tree.print();
    }

    cout << endl << "-------------------------" << endl;
    cout << "All keys are inserted ..." << endl;
    cout << "-------------------------" << endl << endl;

    tree.print();

    shuffle(random_list.begin(), random_list.end(), default_random_engine(r));

    tree.remove(15);
    cout << endl << "-------------" << endl;
    cout << "Removing " << 15 << endl;
    cout << "-------------" << endl << endl;
    tree.print();
    
    for (int i : random_list) {
      tree.remove(i);
      cout << endl << "-------------" << endl;
      cout << "Removing " << i << endl;
      cout << "-------------" << endl << endl;
      tree.print();
    }
    
  }
  */
  return 0;
}