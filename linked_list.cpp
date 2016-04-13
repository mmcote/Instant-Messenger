/*
 linked_list.cpp
  
 This code contains the functions required for using linked lists 
 in the main code. It is a sequence of links where each link holds 
 an item and a pointer to the next/previous link. There are also 
 references to queues and stacks where items are able to be 
 added/removed from the front and back of the list.
*/

#include <Arduino.h>
#include "linked_list.h"

/* Creates an empty linked list and returns a pointer to it */
linked_list l_create() {
  linked_list list = new l_list_t;

  if (list == NULL) {
    Serial.print( "list == NULL" );
  }

  list->front = NULL;
  list->back = NULL;
  list->length = 0;
  return list;
}
//=================================================
/*
  Destroy all links in the list and the list itself,
  freeing up all memory used by the list.
*/
void l_destroy(linked_list list) {
  delete list;
}
//=================================================
/*
  Add the item to the back of the linked list.
*/
void l_add_back(linked_list list, String item, int idx_num) {
  link new_link = new link_t;
  if (new_link == NULL) {
    Serial.print( "new_link == NULL" );
  }
  new_link->item = item;
  new_link->idx_num = idx_num;
  // There will always be nothing after the newest item
  new_link->next = NULL;
  
  if( list->length > 0 )  {
    // Old back becomes new items previous
    new_link->prev = list->back;
    // old list end needs to now point to the new link
    list->back->next = new_link; 
    //the old front now points to the back of the list
    list->back = new_link;

  }
  else
  {
    // always point the new item back to NULL.
    // regardless of whether the list is empty or not
    new_link->prev = list->back;
    // this is the only item in the list, so 
    // have the back point to it as well
    list->back = new_link;
    
    // update the front of the list
    list->front = new_link;
  }
  
  list->length += 1;
}
//=================================================
/*
  Removes the first item held by the list.
*/
void l_remove_front(linked_list list) {
  if(list->length > 1) {
      list->front = list->front->next; // points to next item in list
      list->length -= 1; // decrements size
      list->front->prev = NULL; // points to NULL
  }
  else {
      l_destroy(list); // if the list is equal to 1
  }    
}

