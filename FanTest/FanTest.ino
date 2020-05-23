/*
Atmega644PA Board, External 8Mhz@3.3V
https://github.com/Dafeng1980/AtmegaBoards

*/
const byte OC1A_PIN = 13;        //
const byte OC1B_PIN = 12;
const byte HALLSENSOR = 10;     //INT0 at Mega644P

const word PWM_FREQ_HZ = 25000;     // Adjust this value to adjust the frequency
const word TCNT1_TOP = 8000000/(2*PWM_FREQ_HZ);  //TIME1 
const uint8_t fandiv = 2;  //This is the variable used to select the fan and it's the divider,Set 2 for bipole hall effect sensor

unsigned int NbTopsFan; 
unsigned int fanSpeed;


void rpm ()
//This is the function that the interupt calls
{ NbTopsFan++; }

void setup() {  
  pinMode(OC1A_PIN, OUTPUT);      
  pinMode(HALLSENSOR, INPUT);
  
  Serial.begin(9600);
  attachInterrupt(0, rpm, FALLING);  // RISING or FALLING 

  // Clear Timer1 control and count registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // Set Timer1 configuration
  // COM1A(1:0) = 0b10   (Output A clear rising/set falling)
  // COM1B(1:0) = 0b00   (Output B normal operation)
  // WGM(13:10) = 0b1010 (Phase correct PWM)
  // ICNC1      = 0b0    (Input capture noise canceler disabled)
  // ICES1      = 0b0    (Input capture edge select disabled)
  // CS(12:10)  = 0b001  (Input clock select = clock/1)
  
  TCCR1A |= (1 << COM1A1) | (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << CS10);
  ICR1 = TCNT1_TOP;
  setPwmDuty(0);
}

void loop() { 
    setPwmDuty(30);      //Change this value 0-100 to adjust duty cycle
//    delay(5000);
//    setPwmDuty(50);
//    delay(20000);
 fanSpeed = getfanrpm();
//Prints the number calculated above
Serial.print (fanSpeed, DEC);
//Prints " rpm" and a new line
Serial.print (" rpm\r\n");
}

unsigned int getfanrpm(){
  unsigned int calc;
NbTopsFan = 0;
sei();              //Enables interrupts
delay (1000);
cli(); 
//Times NbTopsFan (which is apprioxiamately the fequency the fan 
//is spinning at) by 60 seconds before dividing by the fan's divider
calc = ((NbTopsFan * 60)/fandiv);
return calc;
}

void setPwmDuty(byte duty) {
  OCR1A = (word) (duty*TCNT1_TOP)/100;
}
