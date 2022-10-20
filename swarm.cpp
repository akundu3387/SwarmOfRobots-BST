#include "swarm.h"
Swarm::Swarm(){//Constructor                                                                                                                         
  m_root = nullptr;
}

Swarm::~Swarm(){//Destructor                                                                                                                         
  clear();
  m_root = nullptr;
}

void Swarm::insert(const Robot& robot){
  m_root = insert(robot,m_root);
  //cout<<"After Insert Check, did it go through???"<<endl;                                                                                          
}


/*                                                                                                                                                   
  Approach/Order for inserting objects:                                                                                                              
  1.)Check if id is less or greater than root, then insert to the left/right accordingly                                                             
  2.)Call UpdateHeight() to Update the height of tree                                                                                                
  3.)Call Rebalance() to rebalance the tree after insertion                                                                                          
 */
Robot* Swarm::insert(const Robot& robot,Robot*& aNode){//Helper function, recursivly iterates tree, inserts object and returns to OG insert()        
  if(aNode == nullptr){//if tree empty, make new node from robot values and return                                                                   
    Robot* aNode = new Robot(robot.m_id,robot.m_type,robot.m_state);
    return aNode;
  }
  else if (aNode->m_id > robot.m_id){//if value is less than node                                                                                    
    aNode->m_left = insert(robot, aNode->m_left);//insert as left child                                                                              
    updateHeight(aNode);
    return rebalance(aNode);
  }
  else if ( aNode->m_id < robot.m_id){//if value is greater than node                                                                                
    //SEG FAULT HERE     **FIXED**                                                                                                                      
    aNode->m_right = insert(robot, aNode->m_right);//insert as right child                                                                           
    updateHeight(aNode);
    return rebalance(aNode);
  }
  else //element exists, no duplicates                                                                                                               
    return aNode;

}

void Swarm::clear(){
  clear(m_root);
}
void Swarm::clear(Robot* aBot){//Helper, called by clear() to recursivly clear the tree                                                              
  if(aBot == nullptr)//tree is already empty                                                                                                         
    return;
  else{
    clear(aBot->m_left);
    clear(aBot->m_right);
    delete aBot;
  }
}

void Swarm::remove(int id){
  remove(m_root,id);

}
Robot* Swarm::remove(Robot* aBot,int id){//Helper, called by remove(int id) to recursivly iterate tree to remove specified object                    
  Robot* temp;
  if(aBot == nullptr)//tree is empty, item not found                                                                                                 
    return nullptr;
  else if(id < aBot->m_id)//traverse left                                                                                                            
    aBot->m_left = remove(aBot->m_left, id);
  else if(id > aBot->m_id)//traverse right                                                                                                           
    aBot->m_right = remove(aBot->m_right, id);
  //case with 2 children                                                                                                                             
  else if(aBot->m_left && aBot->m_right){ //else if here is getting skipped?  **FIXED**                                                              
    //cout<<"test1"<<endl;                                                                                                                           
    temp = findMin(aBot->m_right);//right childs lowest value                                                                                        
    aBot->m_id = temp->m_id;//copy lowest value to robot intended to delete                                                                          
    aBot->m_right = remove(aBot->m_right, aBot->m_id);//delete robot found                                                                           
  }
  else{// the case of zero or one child                                                                                                              
    // also, in the case of two children, after finding right’s lowest                                                                               
    // value we end up here by calling remove function recursively                                                                                   
    temp = aBot;
    //cout<<"test2"<<endl;                                                                                                                           
    if(aBot->m_left == nullptr){
      //cout<<"test3"<<endl;                                                                                                                         
      aBot = aBot->m_right;
    }
    else if(aBot->m_right == nullptr){//SEG FAULT HERE      **FIXED**                                                                                
      //cout<<"test4"<<endl;                                                                                                                         
      aBot = aBot->m_left;
    }
    delete temp;
  }
  updateHeight(aBot);
  aBot = rebalance(aBot);
  return aBot;
}

Robot* Swarm::findMin(Robot* aBot){
  Robot* temp = aBot;
  while(temp->m_left != nullptr){
    temp = temp->m_left;
  }
  return temp;
}
void Swarm::updateHeight(Robot* aBot){
  if(aBot == nullptr)
    return;
  int leftHeight = (aBot->m_left == nullptr ? -1 : aBot->m_left->m_height);
  int rightHeight = (aBot->m_right == nullptr ? -1 : aBot->m_right->m_height);
  aBot->m_height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int Swarm::checkImbalance(Robot* aBot){
  if (aBot == nullptr)
    return -1;
  else{
    int leftHeight = -1;
    int rightHeight = -1;
    if (aBot->m_left != nullptr)
      leftHeight = aBot->m_left->m_height;
    if (aBot->m_right != nullptr)
      rightHeight = aBot->m_right->m_height;
    return (leftHeight - rightHeight);
  }
}
Robot* Swarm::rebalance(Robot* aBot){
  if ((checkImbalance(aBot) < -1) && (checkImbalance(aBot->m_right) <= 0))
    return leftRotation(aBot);//do left rotation at z                                                                                                
  else if ((checkImbalance(aBot) > 1) && (checkImbalance(aBot->m_left) >= 0))
    return rightRotation(aBot);//do right rotation at z                                                                                              
  else if ((checkImbalance(aBot) < -1) && (checkImbalance(aBot->m_right) >=0)){
    //double rotation, right rotation at aNode->m_right, left rotation at aNode                                                                      
    aBot->m_right = rightRotation(aBot->m_right);//right rotation at aNode->m_right, i.e. y                                                          
    return leftRotation(aBot);//left rotation at aNode, i.e. z                                                                                       
  }
  else if ((checkImbalance(aBot) > 1) && (checkImbalance(aBot->m_left) <= 0)){
    //double rotation, left rotation at aNode->m_left, right rotation at aNode                                                                       
    aBot->m_left = leftRotation(aBot->m_left);//left rotation at aNode->m_left, i.e. y                                                               
    return rightRotation(aBot);//right rotation at aNode, i.e. z                                                                                     
  }
  else
    return aBot;
}

Robot* Swarm::leftRotation(Robot* aBot){
  Robot* curr = aBot;
  Robot* right = aBot->m_right;
  curr->m_right = right->m_left;
  right->m_left = curr;
  updateHeight(curr);
  updateHeight(right);
  return right;
}

Robot* Swarm::rightRotation(Robot* aBot){
  Robot* curr = aBot;
  Robot* left = aBot->m_left;
  curr->m_left = left->m_right;
  left->m_right = curr;
  updateHeight(curr);
  updateHeight(left);
  return left;
}

void Swarm::listRobots() const {
  cout<<"\n";
  listRobots(m_root);
}
void Swarm::listRobots(Robot* aBot) const{//performs InOrder traversal                                                                               
  if(aBot != nullptr){
    listRobots(aBot->m_left);//traverse left                                                                                                         
    cout<<aBot->m_id<<" : "<<aBot->getStateStr()<<" : "<<aBot->getTypeStr()<<endl;//visit node                                                       
    listRobots(aBot->m_right);//traverse right                                                                                                       

  }
}

bool Swarm::setState(int id, STATE state){
  Robot* temp;
  if(findBot(id) == false)
    return false;
  else{
    temp = findBot(id,m_root);
    temp->setState(state);
    return true;
  }
}

void Swarm::removeDead(){
  removeDead(m_root);
}
void Swarm::removeDead(Robot* aBot){
  if(aBot != nullptr){
    if(aBot->getState() == DEAD){
      remove(aBot->m_id);
    }
    removeDead(aBot->m_left);
    removeDead(aBot->m_right);
  }
}

bool Swarm::findBot(int id) const {
  Robot* temp = findBot(id,m_root);
  if(temp == nullptr)
    return false;
  if(temp->m_id == id)
    return true;
  else
    return false;
}

Robot* Swarm::findBot(int id, Robot* aBot) const{
  if(aBot == nullptr || aBot->m_id == id)//if bot is either at end of tree or found target bot                                                       
    return aBot;
  else if(aBot->m_id > id)
    return findBot(id,aBot->m_left);//if bots id > targer id, traverse left                                                                          
  else
    return findBot(id,aBot->m_right);//if bots id < target id, traverse right                                                                        

}

void Swarm::dumpTree() const {
  dump(m_root);
}
void Swarm::dump(Robot* aBot) const{
  if (aBot != nullptr){
    cout << "(";
    dump(aBot->m_left);//first visit the left child                                                                                                  
    cout << aBot->m_id << ":" << aBot->m_height;//second visit the node itself                                                                       
    dump(aBot->m_right);//third visit the right child                                                                                                
    cout << ")";
  }
}
