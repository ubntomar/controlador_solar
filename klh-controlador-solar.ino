//las direcciones ip se deben cambia en el setup y en la funcion verifica
#include <EtherCard.h>
#include<stdlib.h>
#include <EEPROM.h>
#define DEBUG 1   // set to 1 to display debug info via seral link
//A6 Y A7 SON SALIDAS DIGITALES
////////////////////////////////////////////////////1213 serial para tarjeta para el mtecri.25 11 2016->192.168.60.226>mac->0x74, 0x69, 0x69, 0x2D, 0x30, 0x37
char              serial_device[]  = "1983";      //1981 serial para tarjeta para el retiro.19 10 2016->192.168.80.250--mac:0x74, 0x69, 0x69, 0x2D, 0x30, 0x3D
                                                  //1982 serial para tarjeta para calizas   12 12 2016->192.168.80.240--mac:0x74, 0x69, 0x69, 0x2D, 0x30, 0x3A
////////////////////////////////////////////////////1983 serial para tarjeta para leo....   12 12 2016->192.168.7.250--mac:0x74, 0x69, 0x69, 0x2D, 0x30, 0x3B
////////////////////////////////////////////////////1984 serial para tarjeta para Rivera.   12 12 2016->192.168.80.244--mac:0x74, 0x69, 0x69, 0x2D, 0x30, 0x3C
#define SERIALA1  "s=1213&v1=0&v2=0&r1=0&r2=0&r3=0&r4=0&rst=0&ip=192.168.001.001&readserver=1"
#define SERIALA2  "s=1213&v1=----&v2=----&r1=0&r2=0&r3=0&r4=0&rst=0&ip=192.168.001.001"
////////////////////////////////////////////////////////////////////////
// Default network config                                             // 
byte              myip[] = {192,168,07,250};                      //
byte              mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x3B};   //
byte              gwip[] = {192,168,07,1};                      //
byte              static_dns[] = {8,8,8,8};                      //
byte              netmask[] = {255,255,255,0};                   //
////////////////////////////////////////////////////////////////////////
// releB controla PANELES  pin 4
// releD controla CARGA    pin 6
////////////////////////////////////////////////////////////////////////
char              serial_temp[] = "9999";
int               sensorPin =  A1;    //sensor modulo de baterias 1 en A1
int               sensorPin2 = A0;   //sensor modulo de baterias 2 en A0
int               ucclockin =  A2; //entrada de pulso de comando de micro pic12f629

int               trigerA =     2;        //OUTPUT activador para lectura sensor de voltaje
int               trigerB =     3;        //OUTPUT activador para lectura sensor de voltaje
int               getreply = 0;
int               repeatstore = 0;
int               flagstore=0;
int               getrequest = 0;
int               timeout_dns = 0;
int releB = 4; int releC = 5; int releD = 6; int releE = 7;
//int               LEDreset28j60= 7;
const char        website[] PROGMEM = "www.redesagingenieria.com";
//#define REQUEST_RATE 1800000 // milliseconds  --30 minutos
#define REQUEST_RATE 900000 //1800000 milliseconds  --30 minutos
#define REQUEST_RATE_REPEAT 20000
int               read_sensor_delay = 100;
byte              Ethernet::buffer[330];//copy and pasted in agc.php    225
char              myArray[0];
static uint32_t   timer;
static uint32_t   timer2;
static uint32_t   timer3;
int               segs=0;
int               contsegs=0;
int               pass=0;
unsigned long     time;
BufferFiller bfill;
int releb_value = 1; int relec_value = 1; int reled_value = 1; int relee_value = 1;

char              ipadd[16];
char              gwadd[16];
char              nmadd[16];
char              part1[4];
char              part2[4];
char              part3[3];
float             sensor1Value;
float             sensor2Value;
char              charVal[10];
char              charVal2[10];               //temporarily holds data from vals
int               dstart = 0;
int               dlen = 0;
const char htmlHeaderphone[] PROGMEM =
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "Pragma: no-cache\r\n"
  "\r\n"
  ;
// ---------------------------------------------
// called when the client request is complete
static void my_result_cb (byte status, word off, word len)
{
  Serial.print(F("<<< reply "));
  Serial.print(millis() - timer);
  Serial.println(" ms");
  //Serial.println((const char*) Ethernet::buffer + off);
  //delay(1000);
  char* data = (char *) Ethernet::buffer + off;
  Serial.println(data);
  Serial.println("--");
  Serial.println(" R?:");
  Serial.println(data[241]);//R
  Serial.println(data[242]);//0  r1
  Serial.println(data[243]);//0  r2
  Serial.println(data[244]);//0  r3
  Serial.println(data[245]);//0  r4
  Serial.println(data[246]);//k
  Serial.println(data[247]);//l
  Serial.println(data[248]);//h
  Serial.println(data[249]);//1  device code 1
  Serial.println(data[250]);//9  device code 2
  Serial.println(data[251]);//8  device code 3
  Serial.println(data[252]);//1  device code 4
 
  myArray[0] = data[241]; //equivale a caracter      echo "R11116789RRRRRRRRRRR
  pass=0;
  if (strncmp("R", myArray, 1) == 0)
  {
    
    pass=0;
    getreply = 1;
    repeatstore = 0;
    flagstore=0;
    Serial.println(F("get reply = 1:)"));
    Serial.println("r1:");
    Serial.println(data[242]);
    Serial.println("r2:");
    Serial.println(data[243]);
    Serial.println("r3:");
    Serial.println(data[244]);
    Serial.println("r4:");
    Serial.println(data[245]);
    Serial.println(F("devie code:"));
    Serial.println(data[249]);
    Serial.println(data[250]);
    Serial.println(data[251]);
    Serial.println(data[252]);
    serial_temp[0] = data[249];
    serial_temp[1] = data[250];
    serial_temp[2] = data[251];
    serial_temp[3] = data[252];
    if (strncmp(serial_device, serial_temp, 4) == 0)
    {
      Serial.println(F("get serial device :)"));
      myArray[0] = data[158];
      if (strncmp("1", myArray, 1) == 0)
      {
        if (digitalRead(releB) == 0)
        {
          digitalWrite(releB, HIGH);
          EEPROM.write(1, 1);
        }
        Serial.println(F(" rele 1 en 1"));
      }
      if (strncmp("0", myArray, 1) == 0)
      {
        if (digitalRead(releB) == 1)
        {
          digitalWrite(releB, LOW);
          EEPROM.write(1, 0);
        }
        Serial.println(F(" rele 1 en 0"));
      }
      myArray[0] = data[159];
      if (strncmp("1", myArray, 1) == 0)
      {
        if (digitalRead(releC) == 0)
        {
          digitalWrite(releC, HIGH);
          EEPROM.write(2, 1);
        }

      }
      if (strncmp("0", myArray, 1) == 0)
      {
        if (digitalRead(releC) == 1)
        {
          digitalWrite(releC, LOW);
          EEPROM.write(2, 0);
        }
      }
      myArray[0] = data[160];
      if (strncmp("1", myArray, 1) == 0)
      {
        if (digitalRead(releD) == 0)
        {
          digitalWrite(releD, HIGH);
          EEPROM.write(3, 1);
        }
      }
      if (strncmp("0", myArray, 1) == 0)
      {
        if (digitalRead(releD) == 1)
        {
          digitalWrite(releD, LOW);
          EEPROM.write(3, 0);
        }

      }
      myArray[0] = data[161];
      if (strncmp("1", myArray, 1) == 0)
      {
        if (digitalRead(releE) == 0)
        {
          digitalWrite(releE, HIGH);
          EEPROM.write(4, 1);
        }
      Serial.println(F(" rele 4 en 1"));
      }
      if (strncmp("0", myArray, 1) == 0)
      {
        if (!digitalRead(releE) == 1)
        {
          digitalWrite(releE, LOW);
          EEPROM.write(4, 0);
        }
      Serial.println(F(" rele 4 en 0"));
      }
    }
  }
  else       {
    getreply = 0;
    repeatstore=1;
    Serial.println(F("get reply = 0:-("));

  }
return;
}

void sensor1() {
  digitalWrite(trigerA, HIGH);
  delay(read_sensor_delay);
  int lectura = analogRead(sensorPin);
  sensor1Value = ((((((lectura) * 5.0) / 1023.0) / 0.0909) - 0.14) / 1.2176)-0.0; //y=mx+b (9.5,9.0)(24.2,26.5)  x=y+2.298 / 1.190 1.190=m
  Serial.println(F("voltaje 1 "));
  Serial.println(lectura);
  Serial.println(sensor1Value);
  digitalWrite(trigerA, LOW);
  delay(read_sensor_delay);
  dtostrf(sensor1Value, 4, 1, charVal);
}
// ----------------------------------------------
void sensor2() {
  delay(read_sensor_delay);  // 17 03 16 --por q la segunda lectura molesta?
  digitalWrite(trigerB, HIGH);
  delay(read_sensor_delay);
  int lectura2 = analogRead(sensorPin2);
  sensor2Value = (((((lectura2 - 57) * 5.0) / 1023.0) / 0.0909) + 2.298) / 1.190;
  if (sensor2Value < 0)
      sensor2Value=0;
  Serial.println(F("voltaje 2"));
  Serial.println(lectura2);
  Serial.println(sensor2Value);
  digitalWrite(trigerB, LOW);
  delay(read_sensor_delay);
  dtostrf(sensor2Value, 4, 1, charVal2);

}
static word homePagephone() {
  sensor1();
  sensor2();
  releb_value  = digitalRead(releB);
  relec_value  = digitalRead(releC);
  reled_value  = digitalRead(releD);
  relee_value  = digitalRead(releE);
  bfill = ether.tcpOffset();
  bfill.emit_p( PSTR (
                  "$F"
                  "$D-$D-$D-$D-$S-$S-7890 \r\n"
                ) , htmlHeaderphone, releb_value, relec_value, reled_value, relee_value, charVal, charVal2) ;
  return bfill.position();

}

void restart() {
  while (1)
  {
    if (digitalRead(ucclockin) == 0) {
      digitalWrite(A3, HIGH);
    }
    if (digitalRead(ucclockin) == 1) {
      digitalWrite(A3, LOW);
    }
  }
}

void ethconfig()
{

  sprintf (ipadd, "%u.%u.%u.%u", myip[0], myip[1], myip[2], myip[3]);  //char ipadd[16];
  Serial.println( "Ip:");
  Serial.println(ipadd);
  sprintf (nmadd, "%u.%u.%u.%u", netmask[0], netmask[1], netmask[2], netmask[3]);  //char ipadd[16];
  Serial.println( "netMask:");
  Serial.println(nmadd);
  sprintf (gwadd, "%u.%u.%u.%u", gwip[0], gwip[1], gwip[2], gwip[3]);  //char ipadd[16];
  Serial.println( "Gw:");
  Serial.println(gwadd);
  ether.hisport = 80 ;
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println(F( "Failed Ethernet  start"));
  Serial.println(F( "probando ether.staticSetup"));  
  if (ether.begin(sizeof Ethernet::buffer, mymac) != 0) {
    ether.staticSetup(myip, gwip, static_dns, netmask);
    Serial.println(F( "probando dns"));
  }
  delay(1000);//prueba de desbloqueo
  dnscheckup();
  Serial.println("DNS Checked.");
}
void dnscheckup()
{
  Serial.println(F("..entrando...dnscheckup().probando DNS..."));
  if (ether.dnsLookup(website))

    while (ether.clientWaitingGw())
    {
      ether.packetLoop(ether.packetReceive());
      if (digitalRead(ucclockin) == 1) {
        digitalWrite(A3, HIGH); //Interface de lectura y escritura entre arduino y pic12f629 para monitorear comunicacion
      }
      if (digitalRead(ucclockin) == 0) {
        digitalWrite(A3, LOW);  //A traves de A2 Lee y A3 devuelve communicacion.
      }
      Serial.println("dns. ok...");
    }



  else  {
    timeout_dns = 1; Serial.println(F("timeout dns.."));

  }
Serial.println(F(".Saliendo...dnscheckup()"));  
}
// ----------------------------------------------
void setup () {
  time = millis();
  timer2 = 0;
  timer3 = 0;
  Serial.begin(9600);
  pinMode(trigerA, OUTPUT); pinMode(trigerB, OUTPUT); pinMode(releB, OUTPUT); pinMode(releC, OUTPUT); pinMode(releD, OUTPUT); pinMode(releE, OUTPUT); //pinMode(LEDreset28j60, OUTPUT);
  pinMode(ucclockin, INPUT); pinMode(A3, OUTPUT); pinMode(0, OUTPUT); pinMode(1, OUTPUT);
  //digitalWrite(LEDreset28j60,LOW);
  digitalWrite(releB, EEPROM.read(1));
  digitalWrite(releC, EEPROM.read(2));
  digitalWrite(releD, EEPROM.read(3));
  digitalWrite(releE, EEPROM.read(4));
  timer = - REQUEST_RATE; // start timing out right away
  Serial.println(F("eeprommm restart value:."));
  Serial.println(EEPROM.read(7));
  if (EEPROM.read(7) == 1)
  {
    EEPROM.write(8, 55);
    EEPROM.write(9, 249);
    EEPROM.write(10, 1);
    EEPROM.write(7, 0);
    restart();
  }
  if (EEPROM.read(7) == 3)  //    kevin leandro
  {
    myip[2] = EEPROM.read(8);
    gwip[2] = EEPROM.read(8);
    myip[3] = EEPROM.read(9);
    gwip[3] = EEPROM.read(10);
    Serial.println(F( "eepromm 8:"));
    Serial.println( EEPROM.read(8));
    Serial.println(F( "eepromm 9:"));
    Serial.println(  EEPROM.read(9));
    Serial.println(F ("eepromm 10:"));
    Serial.println( EEPROM.read(10));

  }
  ethconfig();
}
//------------------------------------------------------------------------------------------------------
void loop () {
  //digitalWrite(releC, HIGH);
  //digitalWrite(releE, LOW);
  //ether.persistTcpConnection(false);
  time = millis();
  // Serial.println(time);
  if (time >= 43200000)
  {
    if (digitalRead(ucclockin) == 0) {
      digitalWrite(A3, HIGH);
    }
    if (digitalRead(ucclockin) == 1) {
      digitalWrite(A3, LOW);
    }
  }
  if (time < 43200000)
  {
    if (digitalRead(ucclockin) == 1) {
      digitalWrite(A3, HIGH); //Interface de lectura y escritura entre arduino y pic12f629 para monitorear comunicacion
    }
    if (digitalRead(ucclockin) == 0) {
      digitalWrite(A3, LOW);  //A traves de A2 Lee y A3 devuelve communicacion.
    }
  }

 if ((millis() > timer3) ) 
    {
    timer3 +=1000; 
    segs+=1;
    contsegs+=1;
    if(segs==(REQUEST_RATE_REPEAT/1000))  //15000 /1000= 15  ----->  15 Segundos.
        segs=0;
    Serial.print("-cnts-:");  
    Serial.print(contsegs);    
    if(contsegs > ((REQUEST_RATE_REPEAT/1000)+10))  //-->15+10-if(contsegs >25
      {
      flagstore=0;  //como soncroniza cada x segundos leer el servidor web?
      Serial.println("-cntsreini-:");  
      Serial.print(contsegs);  
      if(segs==0) 
          pass=1;
      else pass=0;  
      }
    }
  
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                                                   // REQUEST_RATE= 30 minutos
                                                                                   //timer = - REQUEST_RATE; // start timing out right away
  if ((millis() > timer + REQUEST_RATE) )  
    {                                                                              //si pasaron 30 minutos ó timer2 += REQUEST_RATE_REPEAT; opasaron 30 munutos + 1 segundo yrepeatstore==1
    contsegs=0;                                                                    //repeatstore=1;Serial.println(F("get reply = 0:-("));
                                                                                   //if(flagstore==1)repeatstore=1;
   // flagstore=1;//veo q flagstore solo se pone en 1 aquie en esta linea
    repeatstore=0;
    timer = millis();
    getrequest = 1;
    timer2 = 0;
    ethconfig();
    releb_value  = digitalRead(releB);
    relec_value  = digitalRead(releC);
    reled_value  = digitalRead(releD);
    relee_value  = digitalRead(releE);
    sensor1();
    sensor2();
    char tbuff[16];
    memset( &tbuff, 0, sizeof(tbuff));
    char cbuff[] = SERIALA2; //1212(nocv 2015),1213(ene 2016),1981(oct 2016)
    cbuff[2] = serial_device[0];
    cbuff[3] = serial_device[1];
    cbuff[4] = serial_device[2];
    cbuff[5] = serial_device[3];
    //-----------------v1-------------------------
    dtostrf(sensor1Value, 2, 2, tbuff);
    dstart = 10;
    //start of replace
    dlen = 3;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
 
    //-----------------v2-------------------------
    dtostrf(sensor2Value, 2, 2, tbuff);
    dstart = 18;
    //start of replace
    dlen = 3;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    //-----------------r1-------------------------
    dtostrf(releb_value, 2, 2, tbuff);
    dstart = 26;
    //start of replace
    dlen = 0;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    //-----------------r2-------------------------
    dtostrf(relec_value, 2, 2, tbuff);
    dstart = 31;
    //start of replace
    dlen = 0;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    //-----------------r3-------------------------
    dtostrf(reled_value, 2, 2, tbuff);
    dstart = 36;
    //start of replace
    dlen = 0;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    //-----------------r4-------------------------
    dtostrf(relee_value, 2, 2, tbuff);
    dstart = 41;
    //start of replace
    dlen = 0;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    
    dstart = 52;
    //start of replace.
    dlen = 15;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = ipadd[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));

    Serial.println();
    ;
    Serial.print(F("Buffer"));
    Serial.println();
    Serial.print(cbuff);
    Serial.println(); 
    ether.browseUrl(PSTR("/agcontrol/agc.php?"), cbuff, website, my_result_cb);
    Serial.print(F("GET ALMACENAR Enviado"));
  }
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//inincialmente timer=-30 minutos-getrequest=1 quiere decir q ya entre al primer if ya se envio info a la web site
  if ((millis() > timer + timer2 + 10000)  && (1)) //si ya se  un get pero no respuesta , cada x-timer2- segundos intenta reconectar.
  {  
    pass=0;
    contsegs=0;                    //flagstore=1 cada 30 minutos (pongo como comentario arriba)    //repeatstore=0; cada 30 minutos
    if(flagstore==1)repeatstore=1; //flagstore=0;  //como soncroniza cada x segundos leer el servidor web?flagstore=0 tbn cuando recibo datos ok del servidor.
    timer2 += REQUEST_RATE_REPEAT;
    Serial.println();
    Serial.print(F("getreply vale:"));
    Serial.println();
    Serial.print(getreply);
    Serial.println();
    Serial.print(F("repeatstore vale:"));
    Serial.println();
    Serial.print(repeatstore);
    Serial.println();
    delay(1000);
    if (getreply == 0)
      {
      ethconfig();
      getreply=1;
      }
    if ((getreply == 1)  && (1))
        { // OJO AQUI VA EL SERIAL DEL DISPOSITIVO  --cada x segundos genera peticion para leer los comandos de la APP
          //delay(1000);// 18 03 2016
        char cbuff[] = SERIALA1; //1212(nocv 2015),1213(ene 2016),1982(oct 2016)
        cbuff[2] = serial_device[0];
        cbuff[3] = serial_device[1];
        cbuff[4] = serial_device[2];
        cbuff[5] = serial_device[3];
        
        ether.browseUrl(PSTR("/agcontrol/agc.php?"), cbuff, website, my_result_cb);
        Serial.print(F("send GET sin almacenar -->Enviado--->"));
        getreply = 0;
        }
  flagstore=0; 
  //---------->>>>>>CONECTAR Y DESCONECTAR PANELES Y CARGA SEGUN VOLTAJES>>>>>>>-----------------------
  sensor1();
  float baterias=sensor1Value;
  //===================PANELES====D4=================================       
  if (baterias>=12.6)//bateria Cargada:Desconectar paneles
      {
      Serial.println(F("<Bateria Cargada>::desconectar:_/_: Paneles-Abre el circuito.LOW pin4"));   
      digitalWrite(releB, LOW);   
      }
  if (baterias<=12.3)//bateria <NO> Cargada:Conectar paneles y cargando...
      {
      Serial.println(F("<Bateria NO Cargada> :Conectar:___: Paneles.HIGH pin4  "));    
      digitalWrite(releB, HIGH);   
      }    
  //==================CARGA===D6================================
  if (baterias<=10.7)//bateria descargada:desconectar carga
      {
       Serial.println(F("<Bateria Descargada> :desconectar:_/_: Carga-Abrir el circuito.LOW pin 6")); 
       digitalWrite(releD, LOW);//Abrir el circuito.  
      }
  if (baterias>10.7)//Bateria cargando:conectar carga
      {
       Serial.println(F("<<Bateria O.K para suministrar Carga>> :Conectar Carga:___:Dejar cerrado el circuito. HIGH pin 6"));  
       digitalWrite(releD, HIGH);//Cerrado el circuito.  
      } 
  
  
  }
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  word len = ether.packetReceive();// go receive new packets
  word pos = ether.packetLoop(len);// respond to incoming
  //memset( &myipok, 0, sizeof(myipok));
  if (pos) { // check if valid tcp data is received
    // data received from ethernet

    char* data = (char *) Ethernet::buffer + pos;
    Serial.println(F("----------------"));
    Serial.println("data :");
    Serial.println(data);
    Serial.println(F("----------------"));


    if (strncmp("GET /50", data, 7) == 0) {

      ether.httpServerReply(homePagephone()); // send web page data
    }
    if (strncmp("GET /RST", data, 8) == 0) {  //definir si es para version rele de estado solido o rele de bobina
      digitalWrite(releB, HIGH);
      digitalWrite(releC, HIGH);
      digitalWrite(releD, HIGH);
      digitalWrite(releE, HIGH);
      delay(500);
      digitalWrite(releB, LOW);
      digitalWrite(releC, LOW);
      digitalWrite(releD, LOW);
      digitalWrite(releE, LOW);
      EEPROM.write(6, 1);
      ether.httpServerReply(homePagephone()); // send web page data
    }
    if (strncmp("GET /RSTNTW", data, 11) == 0)
    {
      byte myip[] = { 192, 168, 55, 249 };
      EEPROM.write(7, 1);
      ethconfig();
    }
                                                             //correcion:/IP=IPBYTE3.IPBYTE4.GATEWAYBYTE4              
    if (strncmp("GET /IP=", data, 8) == 0) // 055.250.254    /IP=SUBNETBYTE3.IPBYTE4.GATEWAYBYTE4  {--myip[2]--gwip[2]} {--myip[3]} {---gwip[3]}    192.168.55.249    example   55.249.001
    {
      char myipok[] = "192.168.254";
      Serial.println("begin:");
      Serial.println(data[8]);
      dstart = 8;
      //start of replace
      dlen = 11;
      int j = -1;
      for (int i = dstart; i < dstart + dlen; i++)
      {
        j += 1;
        myipok[j] = data[i];
      }
      Serial.println("myipok:");
      Serial.println(myipok);

      int myipok2[2];
      char *p = myipok;
      char *str;
      int i = -1;
      int number;
      while ((str = strtok_r(p, ".", &p)) != NULL) {
        i += 1;
        Serial.println(F("ip recibida:::"));
        Serial.println(str);
        number = atoi( str );
        myipok2[i] = number;
        Serial.println(F("number::"));
        Serial.println(number);
        Serial.println(F("ip en array:::"));
        Serial.println(myipok2[i]);
      }
      EEPROM.write(8, myipok2[0]);
      EEPROM.write(9, myipok2[1]);
      EEPROM.write(10, myipok2[2]);
      EEPROM.write(7, 3);
      ether.httpServerReply(homePagephone()); // send web page data
      Serial.println(F("restart"));
      restart();
    }

  }

}




