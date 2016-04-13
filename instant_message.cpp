#include <Arduino.h>
#include <SD.h> // include the SD card library
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#include <linked_list.h>

/* Instead of using malloc and free we decided to use the c++
implementation and write a substitute for realloc within the
specific functions */

void* operator new[](size_t size) { return malloc(size); }  
void operator delete[](void* ptr) { free(ptr); }

// Standard UA pin settings
#define SD_CS   5
#define TFT_CS  6
#define TFT_DC  7
#define TFT_RST 8

// Joystick
const int VERT = 0; // analog input
const int HORIZ = 1; // analog input
const int SEL = 9; // digital input

// Define the tft
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

/* The struct pointer that points to where the linked discussion list is
stored, here the memory is allocated for 1 discussion_list */
d_list_pointer core = new discussion_list;

// A few of the global variables 
int font_size = 8; // For printing messages

/* assumed amount of memory left after all the variables needed for all
the global and function variables are allocated is 7000 bytes.
Although since reallocating memory will sometimes duplicate allocated 
memory somewhere else then assign that to the new pointer we need to have
double the memoray availible just in case the memory is copied it has room
for the last few bytes allowed. */
int mem_left = 7000;
int vertical; // initial vertical value of resting joystick

// -----------------CONVERSATION SCREEN DRAWING-------------------------

// initially called to draw empty chat screen
void draw_discussion() {
  tft.fillScreen(tft.Color565(0x0000, 0x0000, 0x0000));
  tft.drawRect(0,100,128, 60, 0xFFFF);
  tft.fillRect(0,100,128, 60, 0xFFFF);
}

/* global variables needed to properly navigate and redraw messages
as one navigates the messages*/
int writing_x = 0;  // Where to start writing in x
int writing_y = 100; // Where to start writing in y
int current_top = 0; // current message on top of message nav section
int old_top = 0;  // last message to be on top before changed
int current_bottom = 0;  // current message on bottom of message nav section
int old_bottom = 0; // last message on bottom of message nav section

// Draw characters as one is typing into the chat
void draw_character( char letter, int character) {
  tft.setTextColor(0x0000, 0xFFFF); 
  writing_x = (6*(character%21)); // move along a line while typing
  if ( character%21 == 0 && character != 0) {
    writing_y = writing_y + 8; // move down after 21 characters to new line
  }
  tft.setCursor(writing_x, writing_y);
  tft.print(letter);
}

// Once the message is done clear writing box
void clear_writing_box()  {
  tft.drawRect(0,100,128, 60, 0xFFFF);
  tft.fillRect(0,100,128, 60, 0xFFFF);
  writing_x = 0;  // reset for next message to be written
  writing_y = 100;
}

void write_messages(d_pointer discussion_selected, int selection, bool discussion_nav) {
  if ( discussion_selected->message_count == 1 )  { // for the case when there is only one message
    tft.setTextWrap(true); // Allow wrapping to prevent hidden characters
    tft.setCursor(0,8); //  set the cursor, indicates where to display
    tft.setTextColor( 0x001F, 0x0000); 
    tft.print(discussion_selected->ind_message[0].message);
  }
  else if ( discussion_selected->message_count > 1 ) {
    /* Messages are displayed in groups of 2, never will they be the same 2,
    this is the same as a flipping the page system, although this does account
    for odd messages not in groups of 2 at either end */
    if ( selection%2 != 0 || selection == 1)  {
      selection = selection - 1; // Always has even message on top
    }
    tft.setCursor(0,8); //  set the cursor, indicates where to display
    tft.setTextColor( 0x001F, 0x0000); 
    current_top = selection; 
    if ( current_top != old_top ) { // Only if the message changes does this print again
      tft.fillRect(0,5,128, 38, 0x0000);
      tft.print(discussion_selected->ind_message[current_top].message);
    }
    tft.setCursor(0,48); //  set the cursor, indicates where to display
    tft.setTextColor( 0xF800, 0x0000);
    if ( selection + 1 < discussion_selected->message_count) { // only if there is an odd member to the pair will it print a message on the bottom
      current_bottom = selection + 1;
      if ( current_bottom != old_bottom ) { // Only if the message changes does this print again
        tft.fillRect(0,48,128, 52, 0x0000);
        tft.print(discussion_selected->ind_message[current_bottom].message);      
      }
    }
    else if (current_top != old_top)  {
        tft.fillRect(0,48,128, 52, 0x0000);
        current_bottom = -1;
    }
    old_top = current_top;  // get ready for next loop to check if printed messages have changed
    old_bottom = current_bottom;
  }
}

// -----------------Navigate the old messages---------------------------
int discussion_nav( d_pointer discussion_selected, int selection) {
  int init_joystick_vert;
  int delta_vert;
  delay(300); // Prevent user from scrolling over messages without even seeing them
  init_joystick_vert = analogRead(VERT); // read current position
  delta_vert = vertical - init_joystick_vert;
  int old_selection = selection; // only if selection changes do we redraw the cursor
  bool draw_top = true;
  if ( discussion_selected->message_count > 1 ) {
    if(delta_vert < -300 && (selection + 1 ) < discussion_selected->message_count)  {
      selection = selection + 1; // each loop can only increase selection by 1
    }
    else if (delta_vert > 300 && (selection - 1) >= 0) { // Prevents negatives
      selection = selection - 1; // each loop can only decrease selection by 1
    }
    // mechanism to tell the tft where to draw
    if (discussion_selected->message_count == 1) {
        draw_top = true;  
    }   
    else if (discussion_selected->message_count > 1) {
        if ( selection%2 != 0 || selection == 1 ) { 
            draw_top = false;
        }
    }
    else {
        draw_top = true;
    }
    if(selection != old_selection)  {
        if ( draw_top ) {
          tft.drawLine(0,4,128,4, 0xFFFF);
          tft.drawLine(0,44,128,44, 0x0000);
        }
        else if ( draw_top == false || selection == 1 ) {
          tft.drawLine(0,44,128,44, 0xFFFF);
          tft.drawLine(0,4,128,4, 0x0000);
        }
    }
  }
  else {
      tft.drawLine(0,4,128,4, 0xFFFF);
      selection = 0;
  }
  return selection;
}

// -------------Dynamic character Array to create message---------------
/* Takes char array pointer and reallocates it allow new character to be added to array
this char pointer is needed as it easily creates the string before being assigned to a 
string in the finite state machine */
char* add_character(char* letter, unsigned int* characters, char* permanent) {  
    if (*characters == 0) {
      if (permanent == NULL) {
          Serial.println("Permanent == NULL");
      }
      permanent[0] = *letter;
      *characters = 1;
    }
    else {    // here the reallocation is done
        char* temp = permanent; /* temp pointer pointing to char array, as next we will overwrite
                                this with a new array with the needed size */
        permanent = new char[*characters + 1]; // creates array of same type to specified size
        if (permanent == NULL) {
            Serial.println("Permanent == NULL");
        }
        for (unsigned int i = 0; i < *characters; ++i) {
            permanent[i] = temp[i];
        }
        *characters = *characters + 1;
        permanent[*characters - 1] = *letter;
        delete[] temp; /*temp is deleted as this points to the old char array, therefore as the messages
                        are always being written this will delete old messages in this character buffer*/
    }
  return permanent;
}

// ----------Dynamic string Array to put together discussion------------
messages_pointer add_message(d_pointer discussion_selected, String* output) {
    unsigned int message_count = discussion_selected->message_count;
    if (*output != "") {    // checking for empty String
        if (discussion_selected->ind_message == NULL) {
            message_count = 0;
            discussion_selected->message_count = 1;
            discussion_selected->ind_message = new messages[1];    /* assign to newly allocated memory of 1*discussion size, unlike
                                                                      in the next section, this new discussion does not have to be deleted
                                                                      as this is not a reallocation of memory, just the initial creation
                                                                      of the message array */
            if (discussion_selected->ind_message == NULL) {
                Serial.println("discussion_selected->ind_message = NULL");
            }
            discussion_selected->ind_message[0].message = *output;
        }
        else {    // here the reallocation is done
            messages_pointer temp = discussion_selected->ind_message; /* temp pointer pointing to message_array, as next we will overwrite
                                                                         this with a new array with the needed size */
            discussion_selected->ind_message = new messages[message_count + 1]; // creates random array of same type to specified size
            if (discussion_selected->ind_message == NULL) {
                Serial.println("discussion_selected->ind_message = NULL");
            }
            for (unsigned int i = 0; i < message_count; ++i) {
                discussion_selected->ind_message[i] = temp[i];    /* may seem backwards but the old array was overwritten by the newly
                                                                    allocated space ( new message[num_messages + 1] ), whereas the temp
                                                                    pointer still points to the location of our old array */
            }
            discussion_selected->message_count = message_count + 1;
            discussion_selected->ind_message[message_count].message = *output;
            delete[] temp; // temp is deleted as this points to the old message array
        }
    }
  return discussion_selected->ind_message;
}

void core_init() {
  if (core == NULL) {
      Serial.println("CORE == NULL"); // ensure that the memory was allocated, has an address to point to
    }
    core->length = 0; // Set length to zero to start
}

messages_pointer create_messages() {
    messages_pointer message_array = NULL;    // As there is no elements in our array at this time
    return message_array; // While the message array has not been made yet if there is no messages this should be set to NULL
}


/* Function to create a new discussion in our discussion linked list, this will
create a new discussion link, new discussion, and initialize a message array */
void start_new_discussion(bool main, int assigned_message_number) {
    d_link_pointer temp = new discussion_link;  // Discussion list is created, add first discussion link
    d_pointer temp_discussion = new discussion; // Allocate a discussion to put to the first link
    temp->assigned_message = assigned_message_number;
    temp_discussion->ind_message = create_messages(); // Set message array pointer to NULL
    String temp_title = "discussion"; /* Here the title is set as the program does not yet allow for multiple discussions */
    temp_discussion->title = temp_title; // Set discussion title to given title
    if( core->length == 0 )   { // For first discussion
        temp_discussion->message_count = 0; // zero messages to start
        temp_discussion->discussion_number = 0; // zero indexed discussions
        temp->current_discussion = temp_discussion; // assign new discussion to link
        temp->prev = temp; temp->next = NULL; // Set start previous to self and next to NULL as there is no next
        core->front = temp; core->back = temp; // Set list start and back to one and only discussion
        core->length = 1; // list length 
    }
    else { // For discussions after the first discussion   
        temp_discussion->main = main; //  points to the main
        temp_discussion->discussion_number = core->back->current_discussion->discussion_number + 1; // increments the indexed discussions
        temp->current_discussion = temp_discussion; // assign new discussion to link
        temp->prev = core->back; temp->next = NULL; // sets the previous link to the back of the list and the next to NULL
        core->back->next = temp; // the back of the list now points to the current discussion
        core->back = temp; // sets list to back
        core->length = core->length + 1; // increments the list length
    }
}

/* Function to pick which discussion to point to */
d_link_pointer discussion_finder(int *pick) {
  Serial.print("PICK: "); Serial.println(*pick);
    d_link_pointer iter = core->front; // create a new discussion link that points to the front of the core list
    while (iter != NULL) { // while iter is not empty
        Serial.print("Discussion sought after: "); Serial.print(*pick); 
        Serial.print(" Discussion number matched to: "); Serial.println(iter->current_discussion->discussion_number);
        if (iter->current_discussion->discussion_number == *pick) { // the discussion number of iter and the discussion number sought after are the same
          return iter; 
        }
        else {
          iter = iter->next; // iter points to the next discussion on the list
        }
    }
    return NULL; // if it is empty, return NULL
}

/* Function to calculate the amount of memory left and places limit on number of messages sent and received.
   It returns the bytes left to be able to be used. */
int size_left() {
	// initialize variables
  int mem_taken = 0; 
  int mem_left = 3500; // half of the assumed memory left
  int i = 0;
  String temp;
  
  while(i < core->front->current_discussion->message_count) { // while the counter is less than the message count of the discussion
    temp = (core->front->current_discussion->ind_message[i].message); // assigns the message to an intermediate string
    mem_taken = mem_taken + temp.length() + 1; // calculates the memory taken up by adding to it the length of the string plus the null-terminator ('\0')
    ++i; // increments the counter
    Serial.print("sizeof: "); Serial.println(mem_taken);
  }
  return mem_left - mem_taken;
}

void state()    {
  typedef enum { main_discussion, writing } State; // Two finite states, writing mode and main discussion navigation mode
  State state = writing; // Always start in writing mode as there is zero messages to start
  
  // Recieving Arduino's finite state's
  typedef enum { listen = 0, message_incoming, ack1_from_partner, new_discussion_recieved, ack2_from_partner, message_num_recieved, ack3_from_partner, take_in_discussion_num, ack4_from_partner, take_in_message, ack5_from_partner } Accepting_state;
  Accepting_state accepting_state = listen;

  // Sending Arduino's finite state's
  typedef enum { notification = 0, ack1, new_discussion_send, ack2, message_num_send, ack3, send_discussion_num, ack4, send_message, ack5 } Sending_state;
  Sending_state sending_state = notification;

  d_link_pointer selected_discussion = core->front; // Set initial discussion user is using to the first discussion as there will be only one to start
  int selected_discussion_number = 0; // Zero indexed

  d_link_pointer partner_discussion = core->front;  // Set partner's discussion to input their messages into to the first discussion as both have one to start 
  int partner_discussion_number = 0; // Zero indexed

  char* personal_message = new char;  // Expanding char array for input characters to later transform to a string
  unsigned int personal_chars = 0;  // Amount of characters currently in dynamic char array
  bool new_message_to_send = false; // boolean value to stop the finite state machine from recieving while sending
  String output; // the string the dynamic char array is set to before sent out to the other arduino

  char partner_char;  // Read char value from Serial3 and store in this temp variable
  bool new_message_to_save = false; // New message recieved to store, key to FSM function
  String external_message; // String of message recieved

  linked_list buffer = l_create();  // Create buffer to hold messages ready to send
  unsigned int counter = 0; // To assign link to proper place in buffer list
  start_new_discussion(true, -1); // start new discussion, true as this is a main discussion and -1 as this does not belong to a message
  draw_discussion(); // Draw background to chat

  int selection = 0;  // Where in the discussion navigation the user is viewing

  while(true) {
    if ( state == main_discussion )   { // main discussion state is where one navigates their messages in the discussion
      selection = discussion_nav(selected_discussion->current_discussion, selection); // Selection is incremented and set in discussion_nav
      write_messages(selected_discussion->current_discussion, selection, false);  // Rewrite messsages after selection is changed in discussion_nav
      Serial.println("Main"); // Lets the user confirm where they are on the Serial-mon
      if ( digitalRead(SEL) == LOW )  {
        delay(200); // Prevent going to writing and back before letting go
        state = writing;  // Head back to writing state
      }
    }
    else if ( state == writing && mem_left > 100)  {  // must be at least 100 bytes in order to create one more message
      selection = core->front->current_discussion->message_count - 1; //selection to plot the newest messages when writing as they come in
      char input_char = Serial.read();  // Read partner input
      if ( input_char != -1 && input_char != 13 && personal_chars < 84)  {  // Message limit is 4 lines aka 84 characters
        draw_character(input_char, personal_chars); // Draw characters as they are written
        personal_message = add_character(&input_char, &personal_chars, personal_message); // Add character to dynamic char array
        input_char = Serial.read(); 
      }
      else if ( input_char == 13 || personal_chars == 84) { // Hits enter, or the limit the message will be submitted to other arduino
        input_char = '\0';  // to mark the end of the string
        personal_message = add_character(&input_char, &personal_chars, personal_message); // add end of string marker to dyn array ending
        personal_chars = 0; // set back to zero for next char array
        output = personal_message;  // set to string
        l_add_back ( buffer, output, counter ); // add to message buffer
        new_message_to_send = true; // Tell the user's arduino there is a message to send to our partner
        selected_discussion->current_discussion->ind_message = add_message(selected_discussion->current_discussion, &output);
        clear_writing_box();
        write_messages(selected_discussion->current_discussion, selection, false);
        mem_left = size_left();
        Serial.print("mem_left: "); Serial.print(mem_left); // Display the memory that is left
      }
      write_messages(selected_discussion->current_discussion, selection, false);  // Redraw messages to show the newly written message on user screen
      if ( digitalRead(SEL) == LOW )  {
        state = main_discussion;
        delay(200);
      }
    }
    else { 
      state = main_discussion;
    }

 // !!!!!!!!!!!!! SEND MESSAGE !!!!!!!!!!!!!!!!!!
        if ( new_message_to_send == true )  { // Notify other arduino a message will be coming
            if (sending_state == notification)   {
              Serial.println("Sent New Message Notification");
              Serial3.write('~'); // New Message Identifier
              sending_state = ack1;
            }
            else if (sending_state == ack1 ) {  // Acknowledge their acknowledgement
              Serial.println("Reading Acknowledgement");
                partner_char = Serial3.read();
                if ( partner_char == '@')   { // identifier for acknowledgements
                  Serial.println("Recieved ack1");
                    Serial3.write('@'); // Acknowledge their acknowledgement
                    delay(5);
                    sending_state = send_discussion_num;  // Send the discussion number next for partner knows where to store the given message
                }
            }
            else if (sending_state == send_discussion_num)   {
              Serial.println("Sending Discussion Number");
              // converting integer to char, as this what is expected by partner char
              int x; char buf;  // temp variables
              x = selected_discussion->current_discussion->discussion_number;
              itoa(x, &buf, 10);  
              Serial3.write(buf);
              Serial.print("sent discussion number: "); Serial.println(selected_discussion->current_discussion->discussion_number);
              Serial3.write('%'); // identifier to show the end of some data
              sending_state = ack2;
            }
            else if (sending_state == ack2)  {  // acknowledge their acknowledgement again
              Serial.println("ack2");
                partner_char = Serial3.read();
                if (partner_char == '@')    {
                    delay(5);
                    Serial.println("Sending Acknowledgement");
                    Serial3.write('@');
                    sending_state = send_message;
                }
            }
            else if (sending_state == send_message)   { // Time to send the actual string message
              Serial.println("Sending Message");
              int i = 0;
              Serial3.write('~'); // identifier for the start of some data
              for( i = 0; i < output.length(); ++i)  {
                delay(50);  // Do not overload buffer
                Serial3.write(output[i]);  
              }
              if ( i == output.length() ) {
                Serial3.write('%'); // end of data identifier
              }
              partner_char = Serial3.read();
              if ( partner_char == '@') {
                sending_state = ack3; // acknowledge that they recieved all the data
              }
            }
            else if (sending_state == ack3)  {
              Serial.println("Sending Acknowledgement");
              Serial3.write('@'); // Send acknowledgement
              sending_state = notification; // Send back to start
              new_message_to_send = false;  // Now there is no new messages
              l_remove_front(buffer); // remove from buffer list
        if (new_message_to_send == true)    {
          partner_discussion = discussion_finder(&partner_discussion_number); // find discussion the partner was in when sending the message
          if (partner_discussion == NULL) { // check if this discussion exists
              Serial.print("NULL");
          }
          partner_discussion->current_discussion->ind_message = add_message(partner_discussion->current_discussion, &external_message); // add sent message to dynamic message array
        }
      }
    }
   
     // !!!!!!!!!!!! RECIEVE MESSAGE !!!!!!!!!!!!!!!
        if ( new_message_to_send == false )  {  // Only run if not sending a message
            if (accepting_state == listen)   {  // Always listen for an incoming message
                partner_char = Serial3.read();
                Serial.println("Ready!");
                if ( partner_char != -1 )   {
                    if (partner_char == '~')    { // Message is coming identifier
                        accepting_state = message_incoming;
                    }
                }
            }
            else if (accepting_state == message_incoming) {
                Serial3.write('@'); // acknowledge their notifier
                accepting_state = ack1_from_partner;
                Serial.println("incoming message from partner");
            }
            else if (accepting_state == ack1_from_partner)    {
                partner_char = Serial3.read();
                Serial.println("Waiting for ack1 from partner");
                if (partner_char == '@') {
                    accepting_state = take_in_discussion_num ;
                    Serial.println("recieved ack1, move to take in discussion number");
                }
                else {
                    accepting_state = message_incoming;
                    Serial.println("message still incoming");
                }
            }
            else if ( accepting_state == take_in_discussion_num )  {
                partner_char = Serial3.read();
                Serial.println("waiting for discussion number");
                while (partner_char != '%')  {  // end of data identifier
                    partner_discussion_number = partner_char -'0';  // discussion number, char to int
                    partner_char = Serial3.read();
                    Serial.println("partner discussion number taken");
                }
                Serial3.write('@'); // send acknowledgement for discussion number
                accepting_state = ack2_from_partner;
                Serial.println("moving to acknowledgement 2 from partner");
            }
            else if ( accepting_state == ack2_from_partner )  {
                Serial.println("in acknowledgement 2");
                while ( partner_char != '@')    {
                    partner_char = Serial3.read();
                    if (partner_char == '@') {
                        accepting_state = take_in_message ; // ready to read message
                        Serial.println("retrieved ack 2, moving to take in message");
                    }
                    else {
                        Serial3.write('@');
                    }
                }
            }
            else if ( accepting_state == take_in_message )    {
              char* partner_message = new char; // temporary dynamic char array to take in partner message to later assign to string
              unsigned int partner_chars = 0; // amount of characters
              partner_char = Serial3.read();  
              Serial.println("Taking in message");
              if (partner_char == '~') {  // start of data identifier
                partner_char = Serial3.read();
                while ( partner_char != '%') { // end of data identifier
                  Serial.println("Continued reading serial3.read, partner_char still != '%'");
                  if ( (int) partner_char != -1 )    {
                    partner_message = add_character(&partner_char, &partner_chars, partner_message);  // add character to temp dyn char array
                  }
                  Serial.print("partner_char: "); Serial.println(partner_char);
                  partner_char = Serial3.read();
                }
              }
              if (partner_char == '%') {
                  char temp = '\0'; // end dyn char array with end of string identifier
                  partner_message = add_character(&temp, &partner_chars, partner_message);
                  external_message = partner_message; // assign dyn char array to new string
                  delete [] partner_message;  // delete temp char array
                  new_message_to_save = true; // new message to save as recieving was a sucess
                  accepting_state = ack3_from_partner;
                  Serial3.write('@');
              }
            }
            else if ( accepting_state == ack3_from_partner )  { // final acknowledgement that data-exchange is over
              partner_char = Serial3.read();
              Serial.println("in ack3");
              if (partner_char == '@')    {
                  accepting_state = listen;
                  Serial.println("partner_char is @, moving to listen");
              }
            }
           }
       if (new_message_to_save == true)    {  // save new partner message from recieved data
          partner_discussion = discussion_finder(&partner_discussion_number); // match discussion to what has already been made
          if (partner_discussion == NULL) { // check if this exist
              Serial.print("NULL");
          }
          partner_discussion->current_discussion->ind_message = add_message(partner_discussion->current_discussion, &external_message);
          new_message_to_save = false;
        }
    }
}

/* Main function */
int main()	{
	//Initialise Arduino functionality
	init();
	
    tft.initR(INITR_BLACKTAB);   // initialize a ST7735R chip, black tab
    vertical = analogRead(VERT); // will be 0-1023 ~550
	
	//Attach USB for applicable processors
	#ifdef USBCON
		USBDevice.attach();
	#endif
	
	// Setup for Joystick
    pinMode(SEL, INPUT);
    digitalWrite(SEL,HIGH);
	
	Serial.begin(9600); // Serial 0: communication with PC
	Serial3.begin(9600); // Serial 3: communication with other Arduino

	core_init(); // calls to function to initialize full discussion list
    state(); // calls to finite-state machine 

	Serial.end();
	Serial3.end();
	
    return 0;
}
