int keys[] = {2,3,4,5,6,7,8,9,10,11,12,13};
int outputs[] = {A0, A1, A2, A3};

int keyMappings[12][4] = {
  {0,0,0,1}, {0,0,1,0}, {0,0,1,1}, {0,1,0,0},
  {0,1,0,1}, {0,1,1,0}, {0,1,1,1}, {1,0,0,0},
  {1,0,0,1}, {1,0,1,0}, {1,0,1,1}, {1,1,0,0}
};

bool currentlyPressed[12] = {false};
bool lastButtonState[12] = {HIGH};
unsigned long lastDebounceTime[12] = {0};
unsigned long lastSlotTime = 0;
int currentSlot = 0;

const unsigned long DEBOUNCE_DELAY = 50;  // 50ms антидребезг
const unsigned long SLOT_DURATION = 10;   // 10ms на слот

void setup() {
  for(int i = 0; i < 12; i++) {
    pinMode(keys[i], INPUT_PULLUP);
    lastButtonState[i] = digitalRead(keys[i]);
  }
  for(int i = 0; i < 4; i++) pinMode(outputs[i], OUTPUT);
}

void updateButtonStates() {
  for(int i = 0; i < 12; i++) {
    bool reading = digitalRead(keys[i]);
    
    if(reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }
    
    if((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      if(reading == LOW) {
        currentlyPressed[i] = true;
      } else {
        currentlyPressed[i] = false;
      }
    }
    
    lastButtonState[i] = reading;
  }
}

void loop() {
  updateButtonStates(); 
  
  unsigned long now = millis();
  
  if(now - lastSlotTime >= SLOT_DURATION) {
    lastSlotTime = now;
    
    // Отправляем текущий слот (всегда, даже если нет нажатий)
    if(currentlyPressed[currentSlot]) {
      for(int j = 0; j < 4; j++) {
        digitalWrite(outputs[j], keyMappings[currentSlot][j]);
      }
    } else {
      for(int j = 0; j < 4; j++) {
        digitalWrite(outputs[j], LOW);
      }
    }
    
    currentSlot++;
    if(currentSlot >= 12) currentSlot = 0;
  }
}
