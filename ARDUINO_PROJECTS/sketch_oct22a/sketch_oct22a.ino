int pin = 2;

void setup() {
  // Переключаем контакт GPIO2 в режим вывода данных (OUTPUT):
  pinMode(pin, OUTPUT);
}

// Код в блоке loop() будет повторяться вновь и вновь:
void loop() {
  digitalWrite(pin, LOW);   
                             
  delay(1000);               
  digitalWrite(pin, HIGH);    
                             
  delay(50);               
}
