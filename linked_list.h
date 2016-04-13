#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

typedef struct messages {
    String message;
} messages;
typedef messages* messages_pointer;

// holds array of messages made down below, also holding title and type of discussion (sub or main)
typedef struct discussion {
    messages_pointer ind_message;
    unsigned int message_count;
    int discussion_number;
    String title;
    bool main;
} discussion;
typedef discussion* d_pointer;

// link all the discussions
typedef struct discussion_link {
    int assigned_message;
    d_pointer current_discussion;        // the item being held here
    struct discussion_link* prev;    // point to previous item in the list
    struct discussion_link* next;    // point to next item in the list
} discussion_link;
typedef discussion_link* d_link_pointer;

// full discussion list, choose lists as then it is much easier to insert sub-message titles in menu
typedef struct discussion_list {
    d_link_pointer front; // point to first item in the list
    d_link_pointer back;  // point to last item in the list
    int length;    // number of links in the list
} discussion_list;
typedef discussion_list* d_list_pointer;

// link in the linked list
typedef struct link_t {
  String item;
  int idx_num;
  struct link_t* prev;
  struct link_t* next;
} link_t;
typedef link_t* link;

// linked list struct
typedef struct l_list_t {
  link front;
  link back;
  int length;
} l_list_t;
typedef l_list_t* linked_list;

// creates an empty linked list and returns a pointer to it
linked_list l_create();

// destroys all the links in the list and the list itself
void l_destroy(linked_list list);

// adds item to back of list
void l_add_back(linked_list list, String item, int idx_num);

// removes first item in the list
void l_remove_front(linked_list list); 

#endif
