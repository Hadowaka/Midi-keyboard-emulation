// Arduino Transmitter for Button Decoder
// Отправляет данные на FPGA через 4 пина данных + строб

// Пины кнопок (12 штук)
const int keys[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

// Пины данных (4 бита)
const int dataPins[] = {A0, A1, A2, A3};

// Пин строба
const int strobePin = A4;

// Таблица кодов для каждой кнопки (4 бита)
const int keyMappings[12] = {
  0b0001,  // Key0  - кнопка 0
  0b0010,  // Key1  - кнопка 1
  0b0011,  // Key2  - кнопка 2
  0b0100,  // Key3  - кнопка 3
  0b0101,  // Key4  - кнопка 4
  0b0110,  // Key5  - кнопка 5
  0b0111,  // Key6  - кнопка 6
  0b1000,  // Key7  - кнопка 7
  0b1001,  // Key8  - кнопка 8
  0b1010,  // Key9  - кнопка 9
  0b1011,  // Key10 - кнопка 10
  0b1100   // Key11 - кнопка 11
};

// Состояния кнопок
bool currentlyPressed[12] = {false};
bool lastButtonState[12] = {HIGH};
unsigned long lastDebounceTime[12] = {0};

// Тайминги
const unsigned long DEBOUNCE_DELAY = 50;    // 50ms антидребезг
const unsigned long SLOT_DURATION = 10;     // 10ms на слот (120ms полный цикл)

unsigned long lastSlotTime = 0;
int currentSlot = 0;

void setup() {
  // Настройка кнопок
  for(int i = 0; i < 12; i++) {
    pinMode(keys[i], INPUT_PULLUP);
    lastButtonState[i] = digitalRead(keys[i]);
  }
  
  // Настройка выходов
  for(int i = 0; i < 4; i++) {
    pinMode(dataPins[i], OUTPUT);
    digitalWrite(dataPins[i], LOW);
  }
  
  pinMode(strobePin, OUTPUT);
  digitalWrite(strobePin, LOW);
}

void updateButtonStates() {
  for(int i = 0; i < 12; i++) {
    bool reading = digitalRead(keys[i]);
    
    if(reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }
    
    if((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      currentlyPressed[i] = (reading == LOW);
    }
    
    lastButtonState[i] = reading;
  }
}

void sendSlot() {
  // Получаем код для текущего слота
  int code = currentlyPressed[currentSlot] ? keyMappings[currentSlot] : 0;
  
  // Устанавливаем данные на пины
  for(int j = 0; j < 4; j++) {
    digitalWrite(dataPins[j], (code >> j) & 1);
  }
  
  // Даем время на установку данных
  delayMicroseconds(2);
  
  // Генерируем строб-импульс
  digitalWrite(strobePin, HIGH);
  delayMicroseconds(2);
  digitalWrite(strobePin, LOW);
  
  // Переход к следующему слоту
  currentSlot = (currentSlot + 1) % 12;
}

void loop() {
  updateButtonStates();
  
  if(millis() - lastSlotTime >= SLOT_DURATION) {
    lastSlotTime = millis();
    sendSlot();
  }
}
