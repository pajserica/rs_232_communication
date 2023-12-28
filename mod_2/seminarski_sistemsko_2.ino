// Kod za arduino uno2
#include <SoftwareSerial.h> // Ubacujemo biblioteku koja omogucava da bilo koje pinove pretvorimo u tx i rx
#define BUFFER_SIZE 64 // maximalna velicina buffera u arduinu, od cega su poslednja dva bajta pokazivaci zavrsetka sekvence
#define rxPin 8 
#define txPin 9

SoftwareSerial mySerial(rxPin, txPin); // pravimo novu klasu mySerial i oznacavamo tx i rx pinove

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // definise brzinu slanja i primanja bitova preko serialne konekcije (rs232 protokol);
  mySerial.begin(9600); // definisemo i drugu serialnu konekciju preko koje cemo da komuniciramo sa drugim arduinom.
}

void loop() {
   
  readMySerial();
  readSerial();

}

void changeBaud(char rate){ // prosledjen je int pomocu kojeg odredjujemo jedan od tipicnih baudova 
  int newBaud = 0; 
  switch (rate){ // proveravamo koji je zeljeni baud rate
    case '1': newBaud = 300;
      break;
    case '2': newBaud = 1200;
      break;
    case '3': newBaud = 2400;
      break;
    case '4': newBaud = 4800;
      break;
    case '5': newBaud = 9600;
      break;
    case '6': newBaud = 57600;
      break;
    case '7': newBaud = 115200;
      break;
  }
  
  mySerial.flush(); // Serial flush komanda osigurava da su svi podatci preneti (da je bafer prazan);
  mySerial.begin(newBaud); // ovde oznacavamo da ce se na mySerial terminalu ostvariti newBaud brzina 
  while(mySerial.available()) mySerial.read(); // ovom linijom koda osiguravamo da ako su u toku menjanja bauda poslate informacije, obrisemo iste. 
  Serial.print("baud rate is changed to ");
  Serial.println(newBaud);
}

void atCommand(String message){ // kao argument funkcije uzimamo string da bi smo mogli koristiti funkcije za stringove
  //prva cetiri slova stringa oznacavaju vrstu at komande

  if(message.substring(0,4) == "baud"){ // u ovom slucaju smo izabrali baud komandu 
    char c = message.charAt(4);
    //int temp = atoi(c);
    //Serial.print(temp);
    changeBaud(c); // zovemo funkciju change Baud koja menja baud rate izmedju dva arduina
  }
}

void readMySerial(){
  // procitaj poruku i ispisi na terminalu ako je stigla od drugog mikrokontrolera------------------------------------------------- 1
  while(mySerial.available()){ // proverava da li postoji nesto u baferu (da li je nesto upisano na terminal) od drugog mikrokontrolera, ako ima udji u petlju
    static char message[BUFFER_SIZE - 2]; // deklarisi niz karaktera u koji cemo kasnije upisivati jedan po jedan karakter iz bafera
    static int arrayPos = 0; // pozicija na kojoj pisemo bajt iz bafera u message niz

    char inByte = mySerial.read(); // uzimamo prvi bajt iz bafera i stavljamo ga u inByte varijablu. Takodje bafer se shiftuje u levo za jedan bajt.
    
    // ako uzeti karakter nije jednak karakteru definisanom za zavrsetak poslate poruke (sekvence) 
    // i nismo presli maksimalnu duzinu bafera, udji u blok koda
    if(inByte != '\n' && (arrayPos < BUFFER_SIZE - 1 - 2)){ // minus 3 jer: 2 karaktera su za zavrsetak sekvence i jedan jer niz krece od nule

      message[arrayPos] = inByte; // stavljamo na arrayPos mesto u message nizu inByte karakter.
      arrayPos++; // inkrementiramo arrayPos da ne bi smo svaki put upisivali uzeti karakter na isto mesto;

    }else{ // ako jeste kraj sekvence:

      message[arrayPos] = '\0'; // oznacavamo kraj stringa

      String sMessage = message;
      if(sMessage.substring(0,3) == "AT+"){ // Proverava da li poruka pocinje sa AT+ sto znaci da je to komanda
        Serial.println("AT command received;");
        atCommand(sMessage.substring(3)); // Ulazimo u funkciju koja cita koju at komandu smo izabrali
      }else{
        Serial.print("Stigla je poruka:");
        Serial.println(message);
      }

      arrayPos = 0; // vracamo pokazivac pozicije u stringu na 0 i tako spremamo message niz da primi sledecu poruku

    }

  } 
}

void readSerial(){
  // procitaj poruku ako je stigla sa terminala i posalji je na drugi mikrokontroler ----------------------------------------------------------------------------------- 2
  while(Serial.available()){ // proverava da li postoji nesto u baferu (da li je nesto upisano na terminal), ako ima udji u petlju
    static char message[BUFFER_SIZE - 2]; // deklarisi niz karaktera u koji cemo kasnije upisivati jedan po jedan karakter iz bafera
    static int arrayPos = 0; // pozicija na kojoj pisemo bajt iz bafera u message niz

    char inByte = Serial.read(); // uzimamo prvi bajt iz bafera i stavljamo ga u inByte varijablu. Takodje bafer se shiftuje u levo za jedan bajt.
    
    // ako uzeti karakter nije jednak karakteru definisanom za zavrsetak poslate poruke (sekvence) 
    // i nismo presli maksimalnu duzinu bafera, udji u blok koda
    if(inByte != '\n' && (arrayPos < BUFFER_SIZE - 1 - 2)){ // minus 3 jer: 2 karaktera su za zavrsetak sekvence i jedan jer niz krece od nule

      message[arrayPos] = inByte; // stavljamo na arrayPos mesto u message nizu inByte karakter.
      arrayPos++; // inkrementiramo arrayPos da ne bi smo svaki put upisivali uzeti karakter na isto mesto;

    }else{ // ako jeste kraj sekvence:

      message[arrayPos] = '\0'; // oznacavamo kraj stringa

      String sMessage = message;
      //Serial.println(message);
      if(sMessage.substring(0,3) == "AT+"){ // Proverava da li poruka pocinje sa AT+ sto znaci da je to komanda
        Serial.println("AT command sent;");
        mySerial.println(message);
        delay(30); // cekamo 30 mili sekundi jer zelimo da budemo sigurni da prvo stigne poruka na drugi arduino pa tek onda promenimo baud
        atCommand(sMessage.substring(3)); // Ulazimo u funkciju koja cita koju at komandu smo izabrali
      }else{
        Serial.print("Poslali smo poruku:");
        Serial.println(message);
        mySerial.println(message);  // posalji drugom arduinu 
      }

      arrayPos = 0; // vracamo pokazivac pozicije u stringu na 0 i tako spremamo message niz da primi sledecu poruku

    }

  }
}
  


