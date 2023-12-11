#include <AberLED.h>           //libraries
#include <SPI.h>

#define START 1               //defining states
#define GAME 2
#define PAUSE 3
#define END 4

void setup() {
  AberLED.begin();
  randomSeed(analogRead(A1) + analogRead(A2));             //seed for function rn()
}


 
int state = START;                        

int black = 0;                                 //defining colours used to be put on the board array
int red = 1;
int green = 2;

int playerY = 0;                             
int board[8][8] = {0}; 
int collisions = 0;
int gameSpeed = 250;
int flash = 1;                             //used in the pause function to make the player flash

void takeInput() {
  switch (state) { 
    case START:
      AberLED.swap();
      if (AberLED.getButtonDown(FIRE)) {           //if it detects a button press since the last swap, it will exit and state = game,  and re-initialise variables. 
        state = GAME;
        playerY = 0;
        board[8][8] = {0}; 
        collisions = 0;
        gameSpeed = 250;
        flash = 1;
      }
    break;
    
    case GAME:
      if (AberLED.getButtonDown(2) && playerY < 7) {                //moves player up  
        playerY++;
      } else if (AberLED.getButtonDown(1) && playerY >= 1) {           //moves player down
        playerY--;
      } else if (AberLED.getButtonDown(FIRE)) {                      //will detect if FIRE is pressed, moving the game to the PAUSE state.
        state = PAUSE;
      }
    break;
    
    case PAUSE:
      AberLED.swap();
      if (AberLED.getButtonDown(FIRE)) {                              //unpause the game
        state = GAME;
      }
    break;
    
    case END:                                                      //will await the FIRE button being pressed at the end, which will move to the START state 
      AberLED.swap();
      if (AberLED.getButtonDown(FIRE)) {  
        state = START;
      }
      AberLED.clear();
    break;
    
    default:
    break;
  }
}




int rn(int range) {                      //function generates random integer
  int newRandomNumber = random(range);           
  return newRandomNumber;
}




void update() {       
  int randomRange = 800;              
  
  switch (state) {
    case START:
    break;
    
    case GAME:
      for (int y = 0; y < 8; y++) {             //clears the column at x = 0
        board[0][y] = 0;
      }
   
      for (int x = 0; x < 8; x++) {            //shifts all of the columns values left by 1 in the x direction
        for (int y = 0; y < 8; y++) {
            board[x - 1][y] = board[x][y];
        }
      }
     
      for (int y = 0; y < 8; y++) {          
        int randomN = rn(randomRange);         //reset random number each iteration to generate a new random colour for each dot in the column
        if (randomN <= 64) {                           
          board[7][y] = red;                             //sets colours into board array, depending on the outcome of the rn() function
        } else if (randomN > 64 && randomN <= 80) {
          board[7][y] = green;
        } else { 
          board[7][y] = black;
        }
      }
      
      if (board[0][playerY] == red) {                   //red obstacle collision handling
        gameSpeed -= 50;
        collisions++; 
        if (gameSpeed <= 0 || collisions >= 20) { 
          state = END;
        }
      }
      
      if (board[0][playerY] == green) {                  //green obstacle collision handling
        gameSpeed += 25;
        if (gameSpeed > 250) {
          gameSpeed = 250;                             //limits speed to 250
        }  
      }
    break;
    
    case PAUSE:
      flash *= -1;                           //player changes colour every loop, causing the player to flash in the pause state
    break;
    
    case END:
    break;
    
    default:
    break;
  }
}



void draw() {
  switch (state) { 
    case START:
        AberLED.set(0, 7, GREEN);                              //my start display, which is a green dot in each corner.
        AberLED.set(7, 0, GREEN);
        AberLED.set(0, 0, GREEN);
        AberLED.set(7, 7, GREEN);  
    break;
    
    case GAME:                                             
      AberLED.clear();
      for (int x = 0; x < 8; x++) {                       //runs through each element of the array, determines the colour to be output, and prints it to the display
        for (int y = 0; y < 8; y++) {
           if (board[x][y] == red) {
             AberLED.set(x ,y , RED);
           } else if(board[x][y] == green) {
             AberLED.set(x, y, GREEN); 
           } else if(board[x][y] == black) {
             AberLED.set(x, y, BLACK);
           }
        }
      }
      AberLED.set(0, playerY, YELLOW);                    //displays the player
    
      AberLED.swap();
      delay(gameSpeed); 
    break;
    
    case PAUSE:
      AberLED.clear();
        for (int x = 0; x < 8; x++) {                     //displays the board array(), which remains still because the update function is in the pause state, so will not change the array
          for (int y = 0; y < 8; y++) {
             if (board[x][y] == 1) {
               AberLED.set(x ,y , RED);
             } else if(board[x][y] == 2) {
               AberLED.set(x, y, GREEN); 
             } else if(board[x][y] == 3) {
               AberLED.set(x, y, BLACK);
             }
          }
        }  
        
      if (flash == 1) {                       //alternates between displaying yellow and black in the players position, causing the player to change colour every loop() and flash
        AberLED.set(0, playerY, YELLOW);
        AberLED.swap();
        delay(200);
      } else if (flash == -1)  {
        AberLED.set(0, playerY, BLACK);
        AberLED.swap();
        delay(200);
      }
    break;
    
    case END: 
      AberLED.set(0, 7, RED);                  //end display, which is a red dot in each corner of the screen.
      AberLED.set(7, 0, RED);
      AberLED.set(0, 0, RED);
      AberLED.set(7, 7, RED);
    break;
    
    default:
    break;
  }
}



void loop() { 
    takeInput();                          //runs these 3 functions in order
    update();
    draw();
}
