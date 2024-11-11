void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("hello world!");

}

void loop() {
  // put your main code here, to run repeatedly:
    static uint count = 0;
    count +=1;
    delay(1000);
    Serial.println(count);

}
