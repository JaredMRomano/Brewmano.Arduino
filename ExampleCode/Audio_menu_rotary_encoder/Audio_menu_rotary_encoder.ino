//Inport the libraries
//#include <DFMiniMp3.h>
//LCD config
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address


////////////////////Serial prints for the DFplayer/////////////////////////////
class Mp3Notify
{
public:
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }

  static void OnPlayFinished(uint16_t globalTrack)
  {
    Serial.println();
    Serial.print("Play finished for #");
    Serial.println(globalTrack);   
  }

  static void OnCardOnline(uint16_t code)
  {
    Serial.println();
    Serial.print("Card online ");
    Serial.println(code);     
  }

  static void OnCardInserted(uint16_t code)
  {
    Serial.println();
    Serial.print("Card inserted ");
    Serial.println(code); 
  }

  static void OnCardRemoved(uint16_t code)
  {
    Serial.println();
    Serial.print("Card removed ");
    Serial.println(code);  
  }
};


//DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial);
/////////////////////////////////////////////////////////








//Vectors for musical note and arrow
//uint8_t note[8]  = {0x02, 0x03, 0x02, 0x0e, 0x1e, 0x0c, 0x00, 0x00};
uint8_t arrow[8] = {0x0, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};

//Variables for the menu encoder
int counter = 0; 
int page=1;
int Ready=1;
int submenu=0;
int last_counter = 0; 
bool clk_State;
bool Last_State; 
bool dt_State;  
int pushed = 0;

//The pin for the push button
byte push = 10;

void setup() {
  pinMode (push,INPUT);       //Define the pin as input
  
  lcd.begin(20,4);                 //Init the LCD          
  lcd.createChar(1, arrow);   //Create the arrow symbol
  lcd.home();                 //Home the LCD
  
  PCICR |= (1 << PCIE0);    //enable PCMSK0 scan                                                 
  PCMSK0 |= (1 << PCINT0);  //Set pin D8 trigger an interrupt on state change. 
  PCMSK0 |= (1 << PCINT1);  //Set pin D9 trigger an interrupt on state change.  
  DDRB &= B11111100;        //8, 9 as input for the encoder clock and data pins

  Last_State =   (PINB & B00000001); //pin 8 state (clock pin)? 
  
  //Prepare the DFplayer module comunication and settings  
  Serial.begin(1000000);  
  //mp3.begin();
  //uint16_t volume = mp3.getVolume();
  //mp3.setVolume(20);  
  //uint16_t count = mp3.getTotalTrackCount();

  //Print the initial text. Delete these lines if you don't want that
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write[0];  
  lcd.print("Brewmano");
  lcd.write[0];
  lcd.setCursor(0,1);  
  lcd.print(" Coffee machine ");
  delay(3000);

  //Print the first page menu.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(1);  
  lcd.print("Choose coffee");
  lcd.setCursor(0,1);  
  lcd.print(" Sugar level");
}


//Void for the DFplayer to play sounds
//void waitMilliseconds(uint16_t msWait)
//{
  //uint32_t start = millis();  
 // while ((millis() - start) < msWait)
  //{
  //  mp3.loop(); // calling mp3.loop() periodically allows for notifications to be handled without interrupts
  //  delay(1);
 //   }
 // }

void loop() { 
  
  if((last_counter > counter) || (last_counter < counter)  || pushed) //Only print on the LCD when a step is detected or the button is pushed.
  {
    Ready=1;

  //First page of the menu
    if(submenu == 0)
    {  
      if(0 <= counter && counter < 5)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.write(1);  
        lcd.print("Choose coffee");
        lcd.setCursor(0,1);  
        lcd.print(" Sugar level");
        page=1;
        if(pushed)
        {
         // mp3.playMp3FolderTrack(1);  // play coffe select sound
          pushed=0;
        }
      }
    
      if(5 < counter && counter < 10)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" Choose coffee");
        lcd.setCursor(0,1);
        lcd.write(1);   
        lcd.print("Sugar level");
        page=2;
        if(pushed)
        {
          //mp3.playMp3FolderTrack(2);  
          pushed=0;
        }     
      }
      
      if(10 < counter && counter < 15)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.write(1);  
        lcd.print("Backlight");
        lcd.setCursor(0,1);  
        lcd.print(" Volume");
        page=3;
        if(pushed)
        {
         // mp3.playMp3FolderTrack(3);  
          pushed=0;
        }       
      }
    
      if(15 < counter && counter < 20)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" Backlight");
        lcd.setCursor(0,1);
        lcd.write(1);   
        lcd.print("Volume");
        page=4;
        if(pushed)
        {
          //mp3.playMp3FolderTrack(4);  
          pushed=0;
        }  
     }
  }//submenu = 0;



    //Second page of the menu
    if(submenu == 1)
      {  
        if(0 <= counter && counter < 5)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.write(1);  
          lcd.print("Latte");
          lcd.setCursor(0,1);  
          lcd.print(" Cappuccino");
          page=1;
          pushed=0;    
        }
      
        if(5 < counter && counter < 10)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" Latte");
          lcd.setCursor(0,1);
          lcd.write(1);   
          lcd.print("Cappuccino");
          page=2;
          pushed=0;      
        }
      
        if(10 < counter && counter < 15)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.write(1);  
          lcd.print("Americano");
          lcd.setCursor(0,1);  
          lcd.print(" Back");
          page=3;
          pushed=0;      
        }
      
        if(15 < counter && counter < 20)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" Americano");
          lcd.setCursor(0,1);
          lcd.write(1);   
          lcd.print("Back");
          page=4;
          pushed=0;        
        }
      }//submenu = 1;




    //Third page of the menu
    if(submenu == 2)
    {  
      if(0 <= counter && counter < 5)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.write(1);  
        lcd.print("25%");
        lcd.setCursor(0,1);  
        lcd.print(" 50%");
        page=1;
        pushed=0;    
      }
    
      if(5 < counter && counter < 10)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" 25%");
        lcd.setCursor(0,1);
        lcd.write(1);   
        lcd.print("50%");
        page=2;
        pushed=0;      
      }
    
      if(10 < counter && counter < 15)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.write(1);  
        lcd.print("75%");
        lcd.setCursor(0,1);  
        lcd.print(" 100%");
        page=3;
        pushed=0;      
      }
    
      if(15 < counter && counter < 20)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" 75%");
        lcd.setCursor(0,1);
        lcd.write(1);   
        lcd.print("100%");
        page=4;
        pushed=0;        
      }
    }//submenu = 2;








    //Forth page of the menu
    if(submenu == 3)
    {  
      if(0 <= counter && counter < 5)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.write(1);  
        lcd.print("ON");
        lcd.setCursor(0,1);  
        lcd.print(" OFF");
        page=1;
        pushed=0;    
      }
    
      if(5 < counter && counter < 10)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" ON");
        lcd.setCursor(0,1);
        lcd.write(1);   
        lcd.print("OFF");
        page=2;
        pushed=0;      
      }    
    }//submenu = 3;
  
  
    if(submenu == 4)
    {  
      if(0 <= counter && counter < 5)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.write(1);  
        lcd.print("25%");
        lcd.setCursor(0,1);  
        lcd.print(" 50%");
        page=1;
        pushed=0;    
      }
    
      if(5 < counter && counter < 10)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" 25%");
        lcd.setCursor(0,1);
        lcd.write(1);   
        lcd.print("50%");
        page=2;
        pushed=0;      
      }
    
      if(10 < counter && counter < 15)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.write(1);  
        lcd.print("75%");
        lcd.setCursor(0,1);  
        lcd.print(" 100%");
        page=3;
        pushed=0;      
      }
    
      if(15 < counter && counter < 20)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" 75%");
        lcd.setCursor(0,1);
        lcd.write(1);   
        lcd.print("100%");
        page=4;
        pushed=0;        
      }
    }//submenu = 4;

  
  }//end of the MENU prints on the LCD


  last_counter = counter; //Save the value of the last state



//Now we detect when we push the button
if(digitalRead(push))
  {
     if(submenu == 1)
    {    
       if(page==1)
       {
        submenu=0;
        counter=1;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(5);  // Play sound
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Making coffee...");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       }
    
       if(page==2)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
       // mp3.playMp3FolderTrack(6);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Making coffee...");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       }
    
       if(page==3)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(7);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Making coffee...");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       }
    
       if(page==4)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(8);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" Going back...  ");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(2000);
       }
    }//end of submenu 1






    if(submenu == 2)
    {    
       if(page==1)
       {
        submenu=0;
        counter=1;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(9);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Sugar level 25%");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       }
    
       if(page==2)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(10);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Sugar level 50%");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       }
    
       if(page==3)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(11);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Sugar level 75%");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       }
    
       if(page==4)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(12);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Sugar level 100%");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       }
    }//end of submenu 1





    if(submenu == 3)
    {    
       if(page==1)
       {
        submenu=0;
        counter=1;
        pushed=0;
        Ready=0;
       // mp3.playMp3FolderTrack(13);  // 
        //lcd.backlight();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  Backlight ON  ");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       }
    
       if(page==2)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(14);  // 
        //lcd.noBacklight();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" Backlight  OFF");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       }    
    }//end of submenu 1



    if(submenu == 4)
    {    
       if(page==1)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
       // mp3.playMp3FolderTrack(15);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   Volume 25%   ");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
        //mp3.setVolume(5);  
       }
    
       if(page==2)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(16);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   Volume 50%   ");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       // mp3.setVolume(10);  
       }
    
       if(page==3)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(17);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   Volume 75%   ");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
        //mp3.setVolume(20);  
       }
    
       if(page==4)
       {
        submenu=0;
        counter=0;
        pushed=0;
        Ready=0;
        //mp3.playMp3FolderTrack(18);  // 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   Volume 100%  ");
        lcd.setCursor(0,1);  
        lcd.print("      Wait      ");
        delay(4000);
       // mp3.setVolume(30);  
       }
    }//end of submenu 1











if(submenu == 0 && Ready==1)
    {    
       if(page==1)
       {
        submenu=1;
        counter=0;
        pushed=1;
        //mp3.playMp3FolderTrack(1);  // 
        delay(500);
        
        
       }
    
       if(page==2)
       {
        submenu=2;
        counter=0;
        //mp3.playMp3FolderTrack(2);  // 
        pushed=1;delay(500);
       }
    
       if(page==3)
       {
        submenu=3;
        counter=0;
        //mp3.playMp3FolderTrack(3);  // 
        pushed=1;delay(500);
       }
    
       if(page==4)
       {
        submenu=4;
        counter=0;
        //mp3.playMp3FolderTrack(4);  // 
        pushed=1;delay(500);
       }
    }//end of submenu 0

    
  }


  //Add limit for the counter. Each line of the menu has 5 points. Since my menu has 2 lines the maximum counter will be from 0 to 10
  //If you add more lines for the menu, increase this value
  if(counter > 10)
  {
    counter=10;
  }
  if(counter < 0)
  {
    counter=0;
  }
}//end void



//Interruption vector

ISR(PCINT0_vect){
  
  clk_State =   (PINB & B00000001); //pin 8 state, clock pin? 
  dt_State  =   (PINB & B00000010); 
  if (clk_State != Last_State){     
     // If the data state is different to the clock state, that means the encoder is rotating clockwise
     if (dt_State != clk_State) { 
       counter ++;
     }
     else {
       counter --;
     } 
   } 
   Last_State = clk_State; // Updates the previous state of the data with the current state
  
}
