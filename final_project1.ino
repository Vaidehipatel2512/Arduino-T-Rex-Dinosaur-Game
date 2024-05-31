//the LiquidCrystal library, which is a standard library for interfacing 
// with liquid crystal displays (LCDs) on Arduino.

#include <LiquidCrystal.h>

// Defines the pins that will be used for the display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//bitmap array for the dino character
byte dino [8]
{ B00000,
  B00111,
  B00101,
  B10111,
  B11100,
  B11111,
  B01101,
  B01100,
};

//character for the tree
byte tree [8]
{
  B00011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11111,
  B01110,
  B01110,
};
//button pin
const int BUTTON_ENTER = 8;
const int BUTTON_SELECT = 9;


//Constants for the menu size (MENU_SIZE) and the number of 
//columsn the LCD (LCD_COLUMN) are defined.

const int MENU_SIZE = 2;
const int LCD_COLUMN = 16;

//Constants TREE_CHAR and DINO_CHAR represent the indices assigned to the 
//custom characters for the tree and dinosaur
const int TREE_CHAR = 6;
const int DINO_CHAR = 7;

//Alphabet arrya for our display
const String ALPHABET[26] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

boolean isPlaying = false;//isPlaying indicates whether the game is in progress
boolean isShowScore = false;//isShowScore is used to display high scores
boolean isDinoOnGround = true;//isDinoOnGround tracks whether the dinosaur is on the ground

int currentIndexMenu = 0;
int score = 0;
int scoreListSize = 0;
String scoreList[20];

void setup() {
  lcd.begin(16, 2);//this is called as a setup function of lcd
  lcd.createChar(DINO_CHAR, dino);
  lcd.createChar(TREE_CHAR, tree);

  Serial.begin(9600);
  pinMode(BUTTON_ENTER, INPUT_PULLUP);// means that the pin is intended to read digital input, and the internal pull-up resistor is activated
  pinMode(BUTTON_SELECT, INPUT_PULLUP);//helps to give definite voltage
}

void loop() {
  lcd.clear();//to clear lcd
  handleMenu();

  delay(300);
}


//The handleMenu() function displays a simple menu on the LCD with options "START" and "SCORE."
void handleMenu() {
  
  
  //// Define an array of menu items
  String menu[MENU_SIZE] = { "START", "SCORE" };
//// Loop through each menu item
  for (int i = 0; i < MENU_SIZE; i++) {
    // Loop through each menu item
    if (i == currentIndexMenu) {
      lcd.setCursor(0, i);
      lcd.print("-> ");
    }
 // Print the menu item
    lcd.setCursor(3, i);
    lcd.print(menu[i]);
  }
  
// Check if the "SELECT" button is pressed
  if (digitalRead(BUTTON_SELECT) == LOW) {
    
    // Toggle the current menu selection between 0 and 1
    currentIndexMenu = currentIndexMenu == 0 ? 1 : 0;
  }
// Check if the "ENTER" button is pressed
  if (digitalRead(BUTTON_ENTER) == LOW) {
    
    // If the current menu selection is 0, start the game; otherwise, show the score
    currentIndexMenu == 0 ? startGame() : showScore();
  }
}

void showScore () {
  // Set the flag to indicate that the score is being shown
  isShowScore = true;
  delay(200);
  
//Initialize the index for displaying scores and calculate the last index
  int currentIndex = 0;
  const int lastIndex = scoreListSize - 1;

// Display the initial score
  printScore(currentIndex, lastIndex);
  
  // Continue displaying scores until the flag isShowScore is false

  while (isShowScore) {
    // Check if the "SELECT" button is pressed
    if (digitalRead(BUTTON_SELECT) == LOW) {
 // Move to the next score unless already at the last score, then wrap to the first score
      currentIndex = currentIndex < lastIndex ? currentIndex + 1 : 0;
      printScore(currentIndex, lastIndex);
    }
// Check if the "ENTER" button is pressed to exit the score display
    if (digitalRead(BUTTON_ENTER) == LOW) {
      // Set the flag to end the score display loop
      isShowScore = false;
    }

    delay(200);
  }
}

void printScore(int index, int lastIndex) {
  lcd.clear();
// Check if there are no scores in the list
  if (lastIndex == -1) {
    lcd.print("NO SCORE");
  }
  else {
    // Display the current score
    lcd.print(scoreList[index]);

    // Check if there is a next score to display
    if (index < lastIndex) {
      // Move the cursor to the next line and display the next score
      lcd.setCursor(0, 1);
      lcd.print(scoreList[index + 1]);
    }
  }
}

void startGame () {
  isPlaying = true;
  
// Continue running the game loop as long as isPlaying flag is true
  while (isPlaying) {
    handleGame();
  }
}


void handleGame() {
  lcd.clear();

  int buttonPressedTimes = 0;

  // Generate two random distances for the space between the trees
  int secondPosition = random(4, 9);
  int thirdPosition = random(4, 9);
  int firstTreePosition = LCD_COLUMN;

  const int columnValueToStopMoveTrees = -(secondPosition + thirdPosition);

  // this loop is to make the trees move, this loop waiting until
  // all the trees moved
  for (; firstTreePosition >= columnValueToStopMoveTrees; firstTreePosition--) {

    lcd.setCursor(13, 0);
    lcd.print(score);

    defineDinoPosition();

    int secondTreePosition = firstTreePosition + secondPosition;
    int thirdTreePosition = secondTreePosition + thirdPosition;

    showTree(firstTreePosition);
    showTree(secondTreePosition);
    showTree(thirdTreePosition);

    if (isDinoOnGround) {
      if (firstTreePosition == 1 || secondTreePosition == 1 || thirdTreePosition == 1) {
        handleGameOver();
        delay(5000);
        break;
      }
      buttonPressedTimes = 0;

    } else {
      if (buttonPressedTimes > 3) {
        score -= 3;
      }

      buttonPressedTimes++;
       tone(7,392,300);//G note
       delay(500);
    }

    score++;
    delay(500);
  }
}

void handleGameOver () {
  lcd.clear();
  lcd.print("GAME OVER");
  
  tone(7,261,300);//c (high)note
  delay(300);
  
  tone(7,392,100);//g
  delay(100);

  tone(7,130,200);//c(low)



  lcd.setCursor(0, 1);
  lcd.print("SCORE: ");
  lcd.print(score);

  delay(2000);
  saveScore();
}

void saveScore () {
  lcd.clear();

  String nick = "";
  int nameSize = 0;
  int alphabetCurrentIndex = 0;

  lcd.print("TYPE YOUR NAME");

  while (nameSize != 3) {
    lcd.setCursor(nameSize, 1);
    lcd.print(ALPHABET[alphabetCurrentIndex]);

    if (digitalRead(BUTTON_SELECT) == LOW) {
      alphabetCurrentIndex = alphabetCurrentIndex != 25 ? alphabetCurrentIndex + 1 : 0;
    }

    if (digitalRead(BUTTON_ENTER) == LOW) {
      nick += ALPHABET[alphabetCurrentIndex];

      nameSize++;
      alphabetCurrentIndex = 0;
    }

    delay(300);
  }

  scoreList[scoreListSize] =  nick + " " + score;
  scoreListSize++;

  isPlaying = false;
  score = 0;
}

void showTree (int position) {
  lcd.setCursor(position, 1);
  lcd.write(TREE_CHAR);

  // clean the previous position
  lcd.setCursor(position + 1, 1);
  lcd.print(" ");
}

void defineDinoPosition () {
  int buttonState = digitalRead(BUTTON_ENTER);
  buttonState == HIGH ? putDinoOnGround() : putDinoOnAir();
}

void putDinoOnGround () {
  lcd.setCursor(1, 1);
  lcd.write(DINO_CHAR);
  lcd.setCursor(1, 0);
  lcd.print(" ");

  isDinoOnGround = true;
}

void putDinoOnAir () {
  lcd.setCursor(1, 0);
  lcd.write(DINO_CHAR);
  lcd.setCursor(1, 1);
  lcd.print(" ");

  isDinoOnGround = false;
}