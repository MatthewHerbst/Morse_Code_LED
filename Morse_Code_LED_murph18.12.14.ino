/*
	By Matthew Herbst - Last updated 7/21/2013
	Morse Code LED - Have an LED display the entered text in Morse Code
	Programmed for Arduino with attached breadboard/LED 
	(Easily modified for just the Arduino board by changing the pin)
	GitHub repo: https://github.com/MatthewHerbst/Morse_Code_LED
	Please feel free to contact me with bugs/suggestions:
	herbstmb@miamioh.edu | @WhtHthGdWrought | www.MatthewHerbst.com

 Joe Murphy mods 11/2018.
 I like this code, it just compiled, uploaded and ran.  But it has some issues.
 TODO:
  1) DONE: Looks like a malloc leak.  The linked list is built per character (not sure why not pulled directly from string as yet), but the very tight memory supply runs out very fast as is.
      Also fixed 1st link (root) not being used for a character.
  2) DONE: Put large table (Ardunio wise)  into code space
  3) FIXME: Looks like characters get dropped if I input a long string.  Not sure if that is a function of Arduino limitations or not.
      When I input with strings, I get no characters dropped.  Need to get rid of linked list structure and operate off of simple strings.
  4) DONE: Early on got no warnings on float versus int for DOT / DASH table, but now get warning being converted to int.  Unclear why it is not an int to start with (versus double).
      DONE: Also looks like WORD_SPACE is not being scaled like rest of values.  
      DONE: Also why not scale everything directly in terms of DOTS versus other values.
  5) Add a speaker for playing a tone.
  6) DONE: Bunch of arrays in beginning of program that are never referenced.  Commented them out (even though compiler pretty much took care of it).  Deleted them.
  7) DONE: Added debug so I can make sure lookup table is functioning (got tired of counting dots and dashes).  This means I also dumped any notion of floating point values in calculations.
*/

#include <avr\pgmspace.h>


#define DEBUG YES

//Choose the pin that has the LED connected to it
const int led = 13;

//Morse Code Timings - change as desired
// Given that DOT * MULT * WORD is max value, and is < even 16 bit value, see no reason to convert all these from double to int
const int MULTIPLIER = 1; //Increase durations of dots/dashes
const int DOT = 92 * MULTIPLIER; //1 dot
const int DASH = 3 * DOT * MULTIPLIER; //3 dots
const int TIMING_SPACE = DOT * MULTIPLIER; //1 dot: space between dots/dashes
const int CHAR_SPACE = 3 * DOT * MULTIPLIER; //3 dots: space between characters
const int WORD_SPACE = 7 * DOT * MULTIPLIER; //7 dots - space between words
/*
 * I think it is much better to just read input into a string.  Short program I wrote to test this worked much better (nothing got dropped).
 * So short term will read into a string and then build linked list structure from there.
 * Long term will eliminate linked list structure
 */
#define STRING_LEN  1024
char Input_string[ STRING_LEN ];

//Morse Code timings holder array setup - FIXME: These could be defines since never referenced other than array size
const int NUM_CHARS = 96;
const int MORSE_MAX_CHAR_SIZE = 7;

//Array placement is based on a character's ASCII value
//IF YOU ADD A NEW VALUE BE SURE TO ADD IT TO THE ACCEPTED CHAR ERROR CHECK
//Note: since there is no difference between lower/upper case in Morse, 
//only upper is entered here - lower case is converted to upper using ASCII

// version of table that fits in code space
const int fTIMINGS[NUM_CHARS][MORSE_MAX_CHAR_SIZE] PROGMEM = {
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0},
  {DASH, DOT, DASH, DOT, DASH, DASH, 0}, //!
  {DOT, DASH, DOT, DOT, DASH, DOT, 0}, //"
  {0, 0, 0, 0, 0, 0, 0},
  {DOT, DOT, DOT, DASH, DOT, DOT, DASH}, //$
  {0, 0, 0, 0, 0, 0, 0},
  {DOT, DASH, DOT, DOT, DOT, 0, 0}, //&
  {DOT, DASH, DASH, DASH, DASH, DOT, 0}, //'
  {DASH, DOT, DASH, DASH, DOT, 0, 0}, //(
  {DASH, DOT, DASH, DASH, DOT, DASH, 0}, //)
  {0, 0, 0, 0, 0, 0, 0},
  {DOT, DASH, DOT, DASH, DOT, 0, 0}, //+
  {DASH, DASH, DOT, DOT, DASH, DASH, 0}, //,
  {DASH, DOT, DOT, DOT, DOT, DASH, 0}, //-
  {DOT, DASH, DOT, DASH, DOT, DASH, 0}, //.
  {DASH, DOT, DOT, DASH, DOT, 0, 0}, ///
  {DASH, DASH, DASH, DASH, DASH, 0, 0}, //0
  {DOT, DASH, DASH, DASH, DASH, 0, 0}, //1
  {DOT, DOT, DASH, DASH, DASH, 0, 0}, //2
  {DOT, DOT, DOT, DASH, DASH, 0, 0}, //3
  {DOT, DOT, DOT, DOT, DASH, 0, 0}, //4
  {DOT, DOT, DOT, DOT, DOT, 0, 0}, //5
  {DASH, DOT, DOT, DOT, DOT, 0, 0}, //6
  {DASH, DASH, DOT, DOT, DOT, 0, 0}, //7
  {DASH, DASH, DASH, DOT, DOT, 0, 0}, //8
  {DASH, DASH, DASH, DASH, DOT, 0, 0}, //9
  {DASH, DASH, DASH, DOT, DOT, DOT, 0}, //:
  {DASH, DOT, DASH, DOT, DASH, DOT, 0}, //;
  {0, 0, 0, 0, 0, 0, 0},
  {DASH, DOT, DOT, DOT, DASH, 0, 0}, //=
  {0, 0, 0, 0, 0, 0, 0},
  {DOT, DOT, DASH, DASH, DOT, DOT, 0}, //?
  {DOT, DASH, DASH, DOT, DASH, DOT, 0}, //@
  {DOT, DASH, 0, 0, 0, 0, 0}, //A
  {DASH, DOT, DOT, DOT, 0, 0, 0}, //B
  {DASH, DOT, DASH, DOT, 0, 0, 0}, //C
  {DASH, DOT, DOT, 0, 0, 0, 0}, //D
  {DOT, 0, 0, 0, 0, 0, 0}, //E
  {DOT, DOT, DASH, DOT, 0, 0, 0}, //F
  {DASH, DASH, DOT, 0, 0, 0, 0}, //G
  {DOT, DOT, DOT, DOT, 0, 0, 0}, //H
  {DOT, DOT, 0, 0, 0, 0, 0}, //I
  {DOT, DASH, DASH, DASH, 0, 0, 0}, //J
  {DASH, DOT, DASH, 0, 0, 0, 0}, //K
  {DOT, DASH, DOT, DOT, 0, 0, 0}, //L
  {DASH, DASH, 0, 0, 0, 0, 0}, //M
  {DASH, DOT, 0, 0, 0, 0, 0}, //N
  {DASH, DASH, DASH, 0, 0, 0, 0}, //O
  {DOT, DASH, DASH, DOT, 0, 0, 0}, //P
  {DASH, DASH, DOT, DASH, 0, 0, 0}, //Q
  {DOT, DASH, DOT, 0, 0, 0, 0}, //R
  {DOT, DOT, DOT, 0, 0, 0, 0}, //S
  {DASH, 0, 0, 0, 0, 0, 0}, //T
  {DOT, DOT, DASH, 0, 0, 0, 0}, //U
  {DOT, DOT, DOT, DASH, 0, 0, 0}, //V
  {DOT, DASH, DASH, 0, 0, 0, 0}, //W
  {DASH, DOT, DOT, DASH, 0, 0, 0}, //X
  {DASH, DOT, DASH, DASH, 0, 0, 0}, //Y
  {DASH, DASH, DOT, DOT, 0, 0, 0}, //Z
  {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {DOT, DOT, DASH, DASH, DOT, DASH, 0}, //_
};


//The setup routine runs once when you press reset:
void setup() {                
	//Initialize the digital pin as an output
	pinMode(led, OUTPUT);

	//Initialize Serial
	Serial.begin(9600);
}

//Basic linked-list node implementation 
struct node {
	char value;
	struct node *next;
};

//The loop routine runs in an infinite loop:
void loop() {
  
  int   char_count;        // number of characters read each time
  int   timing_value =0;   // for holding return from lookup table
  char  *str_ptr;           // points to input string.  could use array index, but I like C string semantics better.  Old PDP11 style ;)
  
  
	//Ask the user for the string
	Serial.println("Send text to be displayed in Morse Code");

	//Wait until the user has entered data
	while(Serial.available() == 0);
	char_count = Serial.readBytesUntil( '\n', Input_string, STRING_LEN );
  Serial.print("Count is "); 
  Serial.println( char_count );
  Input_string[ char_count ] = 0;  // terminate 
  Serial.println( Input_string );

  
	//Give Serial a chance to read all the bytes - WTF is this for???
	delay(100);
  
	//Setup the list
	struct node *root, *currentNode, *freeme;
	root = (struct node*)malloc(sizeof(struct node));
	root->value = '\0';
	root->next = 0;
	currentNode = root;

 // start parsing the input string
 str_ptr = Input_string;

	//Read the user input
	while( *str_ptr > 0) {
    // allocate next node in linked list just ahead of use
    currentNode->value = *str_ptr++;  // FIXED: changed this from original - moved this to 1st thing done so root node contains real 1st character.  That way root node gets used to store 1st character.
		currentNode->next = (struct node*)malloc(sizeof(struct node));
		currentNode = currentNode->next;
		currentNode->next = 0;
	}

  // Display the user input back to the user
  // Now start back at beginning of string
	currentNode = root;
	Serial.println("Displaying: ");
	while(currentNode->next != 0) {
		Serial.print(currentNode->value); // FIXED: changed this from original - moved this to 1st thing done so root node gets used
    currentNode = currentNode->next;
	}
	// this is redundent for messages that have \n @ end
	Serial.println();

  // Now start back at beginning of string
	currentNode = root;  
  
	//Go through all  nodes and display characters in Morse Code via the LED
	while(currentNode->next != 0) {


		//Get the ASCII value of the character being looked at
		int ASCII = currentNode->value;

		//Convert lower case letters to upper case
		if(ASCII >= 97 && ASCII <= 122) {
			ASCII = ASCII - 32;
		}
    
		//Ensure the character is one that has been programmed in for Morse
		if((ASCII >= 32 && ASCII <= 34) || ASCII == 36 || 
		   (ASCII >= 38 && ASCII <= 41) || (ASCII >= 43 && ASCII <= 59) ||
		   ASCII == 61 || (ASCII >= 63 && ASCII <= 90) || ASCII == 95) {      
			//If the character is a space
			if(ASCII == 32) {
				delay(WORD_SPACE);
#ifdef DEBUG
        Serial.println();
#endif
			} else {
				//Go through the timing sequence for each character
				for(int i = 0; i < MORSE_MAX_CHAR_SIZE; ++i) {
          // do lookup of next timing - makes it easier to switch between data and code space lookup table
          timing_value = pgm_read_word( &(fTIMINGS[ASCII][i]) );
           
					//If the timing sequence is over, break the loop
					if( timing_value == 0) {
						break;  // hate this contruct - it is another name for GOTO
					} else { //Display the timing sequence on the LED
#ifdef DEBUG
            if ( timing_value == DOT ) {
              Serial.print("DOT ");
            } else if ( timing_value == DASH ) {
              Serial.print("DASH ");
            } else {
              Serial.print("WTF ");
              Serial.print( timing_value );
            }
#endif
						digitalWrite(led, HIGH);
						delay( timing_value );
						digitalWrite(led, LOW);
						delay(TIMING_SPACE);
					}
				}
#ifdef DEBUG
        Serial.println();
#endif
			}
		} else {
			Serial.print("'");
			Serial.print(currentNode->value);
			Serial.print("' does not have a Morse Code timing. Skipping it.");
			Serial.println();
		}
		delay(CHAR_SPACE);

    // Grab current node to free after moving on
    freeme = currentNode;
    //Move to the next character
    currentNode = currentNode->next;
    //FIXED: recover heap space
    free( freeme );
	}
}
