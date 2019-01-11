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
  2) Put large table (Ardunio wise)  into code space
*/

#include <avr\pgmspace.h>
int ledState = LOW; 
int arrayLength = 25; 
const int starPatterns[] PROGMEM = {13,21,25,27,13,39,85,217,84,38,12,20,24,26,12,20,24,26,12,104,138,217,85,39,13}; 
const int speedSettings[] PROGMEM = {1,1,1,1,2,2,2,2,2,2,2,1,1,1,1,1,1,1,2,2,2,2,2,2,2};
const int speedArray[] = {300, 90, 50};
const int sidePatterns[][4] PROGMEM = {{1, 1, 1, 1}, {1, 1, 1, 0}, {1, 1, 0, 1}, {1, 1, 0, 0}, {1, 0, 1, 1}, {1, 0, 1, 0}, {1, 0, 0, 1}, {1, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 1, 0}, {0, 1, 0, 1}, {0, 1, 0, 0}, {0, 0, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}};
const int mainPatterns[][2] PROGMEM = {{0, 1}, {1, 0}, {0, 2}, {2, 0}, {0, 3}, {3, 0}, {0, 4}, {4, 0}, {0, 5}, {5, 0}, {0, 6}, {6, 0}, {0, 7}, {7, 0}, {0, 8}, {8, 0}, {0, 9}, {9, 0}, {0, 10}, {10, 0}, {0, 11}, {11, 0}, {0, 12}, {12, 0}, {0, 13}, {13, 0}, {0, 14}, {14, 0}, {1, 2}, {2, 1}, {1, 3}, {3, 1}, {1, 4}, {4, 1}, {1, 5}, {5, 1}, {1, 6}, {6, 1}, {1, 7}, {7, 1}, {1, 8}, {8, 1}, {1, 9}, {9, 1}, {1, 10}, {10, 1}, {1, 11}, {11, 1}, {1, 12}, {12, 1}, {1, 13}, {13, 1}, {1, 14}, {14, 1}, {2, 3}, {3, 2}, {2, 4}, {4, 2}, {2, 5}, {5, 2}, {2, 6}, {6, 2}, {2, 7}, {7, 2}, {2, 8}, {8, 2}, {2, 9}, {9, 2}, {2, 10}, {10, 2}, {2, 11}, {11, 2}, {2, 12}, {12, 2}, {2, 13}, {13, 2}, {2, 14}, {14, 2}, {3, 4}, {4, 3}, {3, 5}, {5, 3}, {3, 6}, {6, 3}, {3, 7}, {7, 3}, {3, 8}, {8, 3}, {3, 9}, {9, 3}, {3, 10}, {10, 3}, {3, 11}, {11, 3}, {3, 12}, {12, 3}, {3, 13}, {13, 3}, {3, 14}, {14, 3}, {4, 5}, {5, 4}, {4, 6}, {6, 4}, {4, 7}, {7, 4}, {4, 8}, {8, 4}, {4, 9}, {9, 4}, {4, 10}, {10, 4}, {4, 11}, {11, 4}, {4, 12}, {12, 4}, {4, 13}, {13, 4}, {4, 14}, {14, 4}, {5, 6}, {6, 5}, {5, 7}, {7, 5}, {5, 8}, {8, 5}, {5, 9}, {9, 5}, {5, 10}, {10, 5}, {5, 11}, {11, 5}, {5, 12}, {12, 5}, {5, 13}, {13, 5}, {5, 14}, {14, 5}, {6, 7}, {7, 6}, {6, 8}, {8, 6}, {6, 9}, {9, 6}, {6, 10}, {10, 6}, {6, 11}, {11, 6}, {6, 12}, {12, 6}, {6, 13}, {13, 6}, {6, 14}, {14, 6}, {7, 8}, {8, 7}, {7, 9}, {9, 7}, {7, 10}, {10, 7}, {7, 11}, {11, 7}, {7, 12}, {12, 7}, {7, 13}, {13, 7}, {7, 14}, {14, 7}, {8, 9}, {9, 8}, {8, 10}, {10, 8}, {8, 11}, {11, 8}, {8, 12}, {12, 8}, {8, 13}, {13, 8}, {8, 14}, {14, 8}, {9, 10}, {10, 9}, {9, 11}, {11, 9}, {9, 12}, {12, 9}, {9, 13}, {13, 9}, {9, 14}, {14, 9}, {10, 11}, {11, 10}, {10, 12}, {12, 10}, {10, 13}, {13, 10}, {10, 14}, {14, 10}, {11, 12}, {12, 11}, {11, 13}, {13, 11}, {11, 14}, {14, 11}, {12, 13}, {13, 12}, {12, 14}, {14, 12}, {13, 14}, {14, 13}, {14, 14}, {13, 13}, {12, 12}, {11, 11}, {10, 10}, {9, 9}, {8, 8}, {7, 7}, {6, 6}, {5, 5}, {4, 4}, {3, 3}, {2, 2}, {1, 1}, {0, 0}, {15, 15}, {15, 14}, {15, 13}, {15, 12}, {15, 11}, {15, 10}, {15, 9}, {15, 8}, {15, 7}, {15, 6}, {15, 5}, {15, 4}, {15, 3}, {15, 2}, {15, 1}, {15, 0}, {14, 15}, {13, 15}, {12, 15}, {11, 15}, {10, 15}, {9, 15}, {8, 15}, {7, 15}, {6, 15}, {5, 15}, {4, 15}, {3, 15}, {2, 15}, {1, 15}, {0, 15}};


//Choose the pin that has the LED connected to it
const int led = 13;

//Morse Code Timings - change as desired
const double MULTIPLIER = 1.0; //Increase durations of dots/dashes
const double DOT = 92.3 * MULTIPLIER; //1 dot
const double DASH = 276.9 * MULTIPLIER; //3 dots
const double TIMING_SPACE = 92.3 * MULTIPLIER; //1 dot: space between dots/dashes
const double CHAR_SPACE = 276.9 * MULTIPLIER; //3 dots: space between characters
const double WORD_SPACE = 646.1; //7 dots - space between words

//Morse Code timings holder array setup
const int NUM_CHARS = 96;
const int MORSE_MAX_CHAR_SIZE = 7;

//Array placement is based on a character's ASCII value
//IF YOU ADD A NEW VALUE BE SURE TO ADD IT TO THE ACCEPTED CHAR ERROR CHECK
//Note: since there is no difference between lower/upper case in Morse, 
//only upper is entered here - lower case is converted to upper using ASCII
int TIMINGS[NUM_CHARS][MORSE_MAX_CHAR_SIZE] = {
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
  // for holding return from lookup table
  int timing_value =0;
  
	//Ask the user for the string
	Serial.println("Send text to be displayed in Morse Code");

	//Wait until the user has entered data
	while(Serial.available() == 0);

	//Give Serial a chance to read all the bytes
	delay(100);
  
	//Setup the list
	struct node *root, *currentNode, *freeme;
	root = (struct node*)malloc(sizeof(struct node));
	root->value = '\0';
	root->next = 0;
	currentNode = root;

	//Read the user input
	while(Serial.available() > 0) {
    // allocate next node in linked list just ahead of use
    currentNode->value = Serial.read();  // FIXED: changed this from original - moved this to 1st thing done so root node contains real 1st character.  That way root node gets used to store 1st character.
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
			} else {
				//Go through the timing sequence for each character
				for(int i = 0; i < MORSE_MAX_CHAR_SIZE; ++i) {
          // do lookup of next timing - makes it easier to switch between data and code space lookup table
          timing_value = TIMINGS[ASCII][i];
          timing_value = pgm_read_word( &(fTIMINGS[ASCII][i]) );
           
					//If the timing sequence is over, break the loop
					if( timing_value == 0) {
						break;  // hate this contruct - it is another name for GOTO
					} else { //Display the timing sequence on the LED
						digitalWrite(led, HIGH);
						delay( timing_value );
						digitalWrite(led, LOW);
						delay(TIMING_SPACE);
					}
				} 
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
