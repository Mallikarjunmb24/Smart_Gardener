 
#include <Wire.h> 
#include <LiquidCrystal_I2C_Hangul.h>

LiquidCrystal_I2C_Hangul lcd(0x3F,16,2);       //LCD 클래스 초기화

char Disp_Line[17] = "M=000     ARE=00";
char SMS_Str[19] = "M=000 AREA=00 sqft";

const int Seed_Motor_Data1 = 3;
const int ULTRA_FRONT = 5;
const int L_Motor_Data1 = 6;
const int L_Motor_Data2 = 7;
const int R_Motor_Data1 = 8;
const int R_Motor_Data2 = 9;

int i; 
char step_count,step_count_threshold,Moisture_Sensor_Temp,Data_Update_Count,Data_Update_Count_threshold;

char SMS[15]="SMART GARDENER";
char Mb_Num1[11]="8431685554";
//char Mb_Num2[11]="9182036574";
char  Rx_Mob_Num[]="0000000000";
char Control_Z = 0x1A;

void GSM_Init( void );
void GSM_Send_SMS( char *Mb_Num1, int SMS );

char Obstacle_Flag;
String inputString = ""; 
boolean RX_ST_Flag = false; 
boolean stringComplete = false; 


void Forward_L_R( void );
void Rotate_L( void );
void Rotate_R( void );
void Reverse_L_R( void );
void Forward(void);

void Robo_Off(void);
void Seed_M(void);
void cutting(void);


void setup()
{
  lcd.init();
  lcd.backlight();
  delay(500);

  Serial.begin(9600);
  delay(500);

  pinMode(L_Motor_Data1, OUTPUT);
  pinMode(L_Motor_Data2, OUTPUT);
  pinMode(R_Motor_Data1, OUTPUT);
  pinMode(R_Motor_Data2, OUTPUT);
  pinMode(Seed_Motor_Data1, OUTPUT);
  //pinMode(A0, INPUT_PULLUP);  // set pull-up on analog pin 0


  lcd.setCursor(0,0);
  lcd.print( " SMART GARDENER ACTIVE " );
  lcd.setCursor(0,1);
  lcd.print( "    ROBOT       " );
  delay(2000);
  
  GSM_Init( );
  delay(1000);
  lcd.setCursor(0, 1);  
  lcd.print( "   GSM ACTIVE   " );
  delay(2000);

  Forward_L_R( );
  Rotate_L( );
  Rotate_R( );
  Reverse_L_R( );
  Seed_M( );
  
  GSM_Send_SMS(Mb_Num1,SMS);
  delay(6000); 
  lcd.setCursor(0, 1);
  lcd.print( " TEST SMS SENT  " );
  delay(1000);
  
  lcd.clear();
  delay(500);
  
  step_count = 0;
  step_count_threshold = 5;
  Data_Update_Count = 0;
  Data_Update_Count_threshold = 5;
}


void loop() 
{
  for(i=0; i<=3; i++)
  {
    int Moisture_Sensor = analogRead(A0);    
    int Moisture_Sensor_Temp = 500 - ( ( Moisture_Sensor/1024.0 )*5000 ) / 10;
    Disp_Line[4] = (Moisture_Sensor_Temp % 10) + 48;
    Moisture_Sensor_Temp = Moisture_Sensor_Temp / 10;
    Disp_Line[3] = (Moisture_Sensor_Temp % 10) + 48;
    Moisture_Sensor_Temp = Moisture_Sensor_Temp / 10;
    Disp_Line[2] =(Moisture_Sensor_Temp % 10) + 48;
    lcd.setCursor(0,0);
    lcd.print( Disp_Line );
    delay(1000);
  }

  delay(500);
  digitalWrite(Seed_Motor_Data1, HIGH);
  delay(500);
  Forward_L_R( );
  digitalWrite(Seed_Motor_Data1, LOW);
  delay(500);
  
  step_count++;
  Disp_Line[14] = (step_count / 10) + 48;
  Disp_Line[15] = (step_count % 10) + 48;
  lcd.setCursor(0,0);
  lcd.print( Disp_Line );
  delay(1000);
  
  Data_Update_Count++;
  lcd.setCursor(0,0);
  lcd.print( Disp_Line );
  delay(1000);
    
  if( digitalRead(ULTRA_FRONT)== HIGH )
  {
    if(Obstacle_Flag == LOW) 
    {
      Obstacle_Flag = 1;
      Rotate_R( ); 
      Forward( );
      Rotate_R( );  
    }
    else if(Obstacle_Flag == HIGH)
    {
      Obstacle_Flag = 0;
      Rotate_L( );
      Forward( );
      Rotate_L( ); 
    }
  }

  
Data_Update_Count = Data_Update_Count + 1;
 
  if( Data_Update_Count >= 5 )
  {
    Data_Update_Count = 0;
    SMS_Str[4] = Disp_Line[4];
    SMS_Str[3] = Disp_Line[3];
    SMS_Str[2] = Disp_Line[2];

    SMS_Str[12] = Disp_Line[14];
    SMS_Str[13] = Disp_Line[15];
    lcd.setCursor(0,1);
    lcd.print( "   SMS UPDATE   " );
    delay(1000);
    
    GSM_Send_SMS( Mb_Num1, SMS_Str );
    delay(6000);
   
  }

  if( step_count >= 10 )
  {
    Robo_Off();
    lcd.setCursor(0,1);
    lcd.print( "  ROBOT_STOP   " );
    delay(1000);
    GSM_Send_SMS( Mb_Num1, "WORK COMPLETED" );
    delay(6000);

    
    while(1)
    {
        
    }
  } 

}  

void Forward_L_R( void )
{
  digitalWrite(L_Motor_Data1, HIGH);
  digitalWrite(L_Motor_Data2, LOW);
  digitalWrite(R_Motor_Data1, HIGH);
  digitalWrite(R_Motor_Data2, LOW);
  delay(2000);
  digitalWrite(L_Motor_Data1, LOW);
  digitalWrite(R_Motor_Data1, LOW);
  delay(200);
}

void Forward( void )
{
  digitalWrite(L_Motor_Data1, HIGH);
  digitalWrite(L_Motor_Data2, LOW);
  digitalWrite(R_Motor_Data1, HIGH);
  digitalWrite(R_Motor_Data2, LOW);
  delay(1000);
  digitalWrite(L_Motor_Data1, LOW);
  digitalWrite(R_Motor_Data1, LOW);
  delay(500);
}

void Rotate_L( void )
{
  digitalWrite(L_Motor_Data1, HIGH);
  digitalWrite(L_Motor_Data2, LOW);
  digitalWrite(R_Motor_Data1, LOW);
  digitalWrite(R_Motor_Data2, HIGH);
  delay(2150);
  digitalWrite(L_Motor_Data1, LOW);
  digitalWrite(R_Motor_Data2, LOW);
  delay(500);
}

void Rotate_R( void )
{
  digitalWrite(L_Motor_Data1, LOW);
  digitalWrite(L_Motor_Data2, HIGH);
  digitalWrite(R_Motor_Data1, HIGH);
  digitalWrite(R_Motor_Data2, LOW);
  delay(2150);
  digitalWrite(L_Motor_Data2, LOW);
  digitalWrite(R_Motor_Data1, LOW);
  delay(500);
}

void Reverse_L_R( void )
{
  digitalWrite(L_Motor_Data1, LOW);
  digitalWrite(L_Motor_Data2, HIGH);
  digitalWrite(R_Motor_Data1, LOW);
  digitalWrite(R_Motor_Data2, HIGH);
  delay(2000);
  digitalWrite(L_Motor_Data2, LOW);
  digitalWrite(R_Motor_Data2, LOW);
  delay(500);
}

void Robo_Off(void)
{
  digitalWrite(L_Motor_Data1, LOW);
  digitalWrite(L_Motor_Data2, LOW);
  digitalWrite(R_Motor_Data1, LOW);
  digitalWrite(R_Motor_Data2, LOW);
  delay(500);
}

void Seed_M(void)
{
  digitalWrite(Seed_Motor_Data1, HIGH);
  delay(2000);
  
  digitalWrite(Seed_Motor_Data1, LOW);
  delay(500);
}
void cutting( void )
{
  digitalWrite(L_Motor_Data1, HIGH );
  digitalWrite(L_Motor_Data2, LOW);
  digitalWrite(R_Motor_Data1, HIGH);
  digitalWrite(R_Motor_Data2, LOW);
  digitalWrite(Seed_Motor_Data1, HIGH);
  delay(2000);
  digitalWrite(L_Motor_Data1, LOW);
  digitalWrite(R_Motor_Data1, LOW);
  digitalWrite(Seed_Motor_Data1, LOW);
  delay(500);
}

void GSM_Init( void )
{
      Serial.println("AT\r");
      delay(1000); 
}

void GSM_Send_SMS( char *Mb_Num1, char *SMS )  //This Function sends a SMS to a Mob No.
{
  int i;
  delay(500);
  Serial.print("AT+CMGS=\"");
  delay(500);
  for(i=0; *Mb_Num1 != '\0'; i++)
  {
    Serial.print(*Mb_Num1);
    Mb_Num1++;
    delay(50);
  }
  Serial.println("\"");
  delay(500);
  Serial.print("\r");
  for(i=0; *SMS != '\0'; i++)
  {
    Serial.print(*SMS);
    SMS++;
    delay(50);
  }
  delay(500);
  Serial.write(0x1A);
  delay(2000);
}

void serialEvent() 
{

  while (Serial.available() ) 
  {
    char inChar = (char)Serial.read();
    inputString += inChar;       // add it to the inputString:
    RX_ST_Flag = true;
    stringComplete = true;
  }
  
}
