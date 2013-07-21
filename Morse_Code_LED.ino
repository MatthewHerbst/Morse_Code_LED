/*
	By Matthew Herbst - Last updated 6/12/2013
	Morse Code LED - Have an LED display the entered text in Morse Code
	Programmed for Arduino with attached breadboard/LED 
	(Easily modified for just the Arduino board by changing the pin)
	GitHub repo: https://github.com/WhtHthGdWrought/Arduino
	Please feel free to contact me with bugs/suggestions:
	herbstmb@miamioh.edu | @WhtHthGdWrought | www.MatthewHerbst.com
*/
 
//Choose the pin that has the LED connected to it
const int led = 13;

//Morse Code Timings - change as desired
const double MULTIPLIER = 2.5; //Increase durations of dots/dashes
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
	//Ask the user for the string
	Serial.println("Send text to be displayed in Morse Code");

	//Wait until the user has entered data
	while(Serial.available() == 0);

	//Give Serial a chance to read all the bytes
	delay(100);
  
	//Setup the list
	struct node *root, *currentNode;
	root = (struct node*)malloc(sizeof(struct node));
	root->value = '\0';
	root->next = 0;
	currentNode = root;

	//Read the user input
	while(Serial.available() > 0) {
		currentNode->next = (struct node*)malloc(sizeof(struct node));
		currentNode = currentNode->next;
		currentNode->value = Serial.read();
		currentNode->next = 0;
	}
	currentNode = root;
  
	//Display the user input back to the user
	Serial.println("Displaying: ");
	while(currentNode->next != 0) {
		currentNode = currentNode->next;
		Serial.print(currentNode->value);
	}
	Serial.println();
	currentNode = root;  
  
	//Go through all  nodes and display characters in Morse Code via the LED
	while(currentNode->next != 0) {
		//Move to the next character
		currentNode = currentNode->next;

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
					//If the timing sequence is over, break the loop
					if(TIMINGS[ASCII][i] == 0) {
						break;
					} else { //Display the timing sequence on the LED
						digitalWrite(led, HIGH);
						delay(TIMINGS[ASCII][i]);
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
	}
}