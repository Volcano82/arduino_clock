// Defineerd vaste waarden voor rotaryknop
#define ROTARYPA 2
#define ROTARYPB 4
int vorige;
int langs;
int langs2;
int menuitem;
int menuitemdata[9];

long rotaryCount = 0;
long lastRotaryCount = 0;

// Defineerd waarden voor het led scherm
const int clock = 7;
const int data = 8;
const int buttonPin = 3;  

int buttonState = 0;    
int tijdteller  = 0;

                     /*0*/ /*1*/ /*2*/ /*3*/ /*4*/ /*5*/ /*6*/ /*7*/ /*8*/ /*9*/
uint8_t digits[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f };

int     loaddata[][4] = {
                   {1,  0,  0,  0},
                   {0,  1,  0,  0},
                   {0,  0,  1,  0},
                   {0,  0,  0,  1},
                   {0,  0,  0,  2},
                   {0,  0,  0,  4},
                   {0,  0,  0,  8},
                   {0,  0,  8,  0},
                   {0,  8,  0,  0},
                   {8,  0,  0,  0},
                   {16,  0,  0,  0},
                   {32,  0,  0,  0}
};

void setup()
{
 // setupInterrupt();
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(buttonPin, INPUT);
  start();
  writeValue(0x8c);
  stop();

  // clear display
  write(0x00, 0x00, 0x00, 0x00);

  // Voor de draaiknop 
  pinMode(ROTARYPA, INPUT);
  pinMode(ROTARYPB, INPUT);  
  
  digitalWrite (ROTARYPA, HIGH);   // enable pull-up
  digitalWrite (ROTARYPB, HIGH);   // enable pull-up
  
  Serial.begin(9600);
  

}

void loop() {
 
      if (menuitem == 0 && rotaryCount ==   -1  ) {rotaryCount =  0   ;}
      if (menuitem == 0 && rotaryCount ==   10  ) {rotaryCount =  9   ;}
      if (menuitem == 1 && rotaryCount ==   5   ) {rotaryCount =  4   ;}
      if (menuitem == 1 && rotaryCount ==   -1  ) {rotaryCount =  0   ;}
      if (menuitem == 1) { 
          start();
            writeValue((0x8c - 4) + (menuitemdata[1] ));
          stop();
       ;}
      if (menuitem == 2 && rotaryCount ==   13  ) {rotaryCount =  12  ;}
      if (menuitem == 2 && rotaryCount ==  -13  ) {rotaryCount = -12  ;}
      if (menuitem == 3 && rotaryCount ==  3  ) {scroll(300,1,"sven is de beste - yeee") ;}    
      if (rotaryCount == 100 ) {rotaryCount = 99 ;}

      if (vorige == 3)  {
        write(digits[menuitem],9,digits[(rotaryCount / 10) % 10],digits[rotaryCount % 10]);
      }
     
      // Leest draaiknop uit
      leesrotary ();

      //Leest drukknop uit
        if (digitalRead(buttonPin) == HIGH) {
            load(30,1);
            
            if(menuitem != 0) {
                menuitemdata[menuitem] = rotaryCount;
                menuitem =0;  
                rotaryCount = 0;
            } else {
              menuitem = rotaryCount;
              rotaryCount = menuitemdata[menuitem];
            }
               write(digits[menuitem],9,digits[(rotaryCount / 10) % 10],digits[rotaryCount % 10]);
        }   

        tijdteller++;
        if(tijdteller = 100000) {
        
        }
}

void leesrotary () {
      
      
      if(digitalRead (ROTARYPB) == 1 && digitalRead (ROTARYPA) == 0 && vorige != 1) {
        vorige = 1;
        langs  = 1;
     }
    
    if(digitalRead (ROTARYPB) == 0 && digitalRead (ROTARYPA) == 0  && vorige != 2) {
        vorige = 2;
        langs2 = 1;
     }
    
    if(digitalRead (ROTARYPB) == 0 && digitalRead (ROTARYPA) == 1  && vorige != 3) {
        vorige = 3;
        if(langs == 1) {langs = 0; langs2 = 0; rotaryCount++;}
        if(langs == 0 && langs2 == 1) {langs = 0; langs2 =0; rotaryCount--;}
     }

    if(digitalRead (ROTARYPB) == 1 && digitalRead (ROTARYPA) == 1 ) {
        vorige = 4;
     }


    menuitemdata[menuitem] = rotaryCount;
  

  
}

void load(int snelheid, int aantalkeer) {
  for(int j = 0; j < aantalkeer; j++){
      for(int i = 0; i < 12; i++){
        write(loaddata[i][0], loaddata[i][1], loaddata[i][2], loaddata[i][3]);
        delay(snelheid);
      }
  }
}


void scroll(int snelheid, int aantalkeer, String tekst) {
  tekst = "    " + tekst;
  tekst += "    ";

  int tekstLengte = tekst.length()+1;
 
  int eersteplek;
  int tweedeplek;
  int derdeplek;
  int vierdeplek;

  for(int j = 0; j < (tekstLengte-4); j++){
      
      eersteplek = convert2LCD(tekst.substring(j,j+1));
      tweedeplek = convert2LCD(tekst.substring(j+1,j+2));
      derdeplek  = convert2LCD(tekst.substring(j+2,j+3));
      vierdeplek = convert2LCD(tekst.substring(j+3,j+4));

        write(eersteplek,tweedeplek, derdeplek, vierdeplek);
        delay(snelheid);
      
  }
}

void write(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth) {
  start();
  writeValue(0x40);
  stop();

  start();
  writeValue(0xc0);
  writeValue(first);
  writeValue(second);
  writeValue(third);
  writeValue(fourth);
  stop();
}

void start(void) {
  digitalWrite(clock,HIGH);//send start signal to TM1637
  digitalWrite(data,HIGH);
  delayMicroseconds(5);

  digitalWrite(data,LOW);
  digitalWrite(clock,LOW);
  delayMicroseconds(5);
}

void stop(void) {
  digitalWrite(clock,LOW);
  digitalWrite(data,LOW);
  delayMicroseconds(5);

  digitalWrite(clock,HIGH);
  digitalWrite(data,HIGH);
  delayMicroseconds(5);
}

bool writeValue(uint8_t value) {
  for(uint8_t i = 0; i < 8; i++)
  {
    digitalWrite(clock, LOW);
    delayMicroseconds(5);
    digitalWrite(data, (value & (1 << i)) >> i);
    delayMicroseconds(5);
    digitalWrite(clock, HIGH);
    delayMicroseconds(5);
  }

  // wait for ACK
  digitalWrite(clock,LOW);
  delayMicroseconds(5);

  pinMode(data,INPUT);

  digitalWrite(clock,HIGH);
  delayMicroseconds(5);

  bool ack = digitalRead(data) == 0;

  pinMode(data,OUTPUT);

  return ack;
}

void rotaryTurn() {
  // Interrupt Service Routine for a change to Rotary Encoder pin A
    
    if (digitalRead (ROTARYPB)) 
     rotaryCount--;   // Turn it Down!
  else 
     rotaryCount++;   // Turn it Up!

} 

int convert2LCD (String teken) {
  int vertaald = 0;

  if (teken == "a") {vertaald = 119;};
  if (teken == "b") {vertaald = 124;};
  if (teken == "c") {vertaald = 57;};
  if (teken == "d") {vertaald = 94;};
  if (teken == "e") {vertaald = 121;};
  if (teken == "f") {vertaald = 113;};
  if (teken == "g") {vertaald = 61;};
  if (teken == "h") {vertaald = 118;};
  if (teken == "i") {vertaald = 6;};
  if (teken == "j") {vertaald = 14;};
  if (teken == "k") {vertaald = 112;};
  if (teken == "l") {vertaald = 56;};
  if (teken == "m") {vertaald = 0;};
  if (teken == "n") {vertaald = 84;};
  if (teken == "o") {vertaald = 92;};
  if (teken == "p") {vertaald = 115;};
  if (teken == "q") {vertaald = 103;};
  if (teken == "r") {vertaald = 80;};
  if (teken == "s") {vertaald = 109;};
  if (teken == "t") {vertaald = 120;};
  if (teken == "u") {vertaald = 62;};
  if (teken == "v") {vertaald = 62;};
  if (teken == "w") {vertaald = 0;};
  if (teken == "x") {vertaald = 0;};
  if (teken == "y") {vertaald = 110;};
  if (teken == "z") {vertaald = 91;};
  if (teken == "1") {vertaald = 6;};
  if (teken == "2") {vertaald = 91;};
  if (teken == "3") {vertaald = 79;};
  if (teken == "4") {vertaald = 102;};
  if (teken == "5") {vertaald = 109;};
  if (teken == "6") {vertaald = 125;};
  if (teken == "7") {vertaald = 7;};
  if (teken == "8") {vertaald = 127;};
  if (teken == "9") {vertaald = 111;};
  if (teken == "0") {vertaald = 63;};
  if (teken == "(") {vertaald = 57;};
  if (teken == ")") {vertaald = 15;};
  if (teken == "-") {vertaald = 64;};
  if (teken == " ") {vertaald = 0;};
  
  return  vertaald;

}

