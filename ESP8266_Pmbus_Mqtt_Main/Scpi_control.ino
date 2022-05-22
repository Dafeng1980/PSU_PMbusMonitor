void sendscpi(char *msg) {
  Serial1.println(msg);
  delay(10);
  Serial.println("send scpi msg");
  if(mqttflag) client.publish("rrh/scpi/info", msg);
}

void setdynload(){
  Serial1.println("*rst");
  delay(20);
  Serial1.println("mode 3");
  delay(20);
  Serial1.println("curr:dyn:l1 15");
  delay(20);
  Serial1.println("curr:dyn:l2 79");
  delay(20);
  Serial1.println("curr:dyn:rise 1");
  delay(20);
  Serial1.println("curr:dyn:fall 1");
  delay(20);
  Serial1.println("curr:dyn:t1 1ms");
  delay(20);
  Serial1.println("curr:dyn:t2 1ms");
  delay(20);
  Serial1.println("load 1");
  delay(20);  
}
