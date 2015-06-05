import processing.serial.*;

int CELLS = 20;
int SCALE = 16;
int MEMORY = 60;

color ON = color(255, 255, 255);
color OFF = color(0, 0, 0);

color[][] board;

int lf = '\n';    // Linefeed in ASCII
int port = 1;
Serial serial;  // The serial port

void setup() {
  MEMORY = floor(displayHeight / SCALE);

  // set up serial communication

  // List all the available serial ports
  println(Serial.list());
  // Open the port you are using at the rate you want:
  serial = new Serial(this, Serial.list()[port], 9600);
  serial.clear();

  // intialize visualization

  board = new color[CELLS][MEMORY];
  for (int x = 0; x < CELLS; x++) {
    for (int y = 0; y < MEMORY; y++) {
      board[x][y] = color(0);
    }
  }

  size(CELLS * SCALE, MEMORY * SCALE);
  noStroke();
}

void draw() {
  while (serial.available() > 0) {
    String input = serial.readStringUntil(lf);
    if (input != null && input.length() >= 20) {
      print(input);
      update(input);
      serial.clear();
    }
  }

  render();
}

void update(String input) {
  // scroll everything up
  for (int y = 0; y < MEMORY - 1; y++) {
    for (int x = 0; x < CELLS; x++) {
      board[x][y] = board[x][y + 1];
    }
  }

  // fill in bottom row
  for (int i = 0; i < CELLS; i++) {
    if (input.charAt(i) == '1') {
      board[i][MEMORY - 1] = color(255);
    } else {
      board[i][MEMORY - 1] = color(0);
    }
  }
}

void render() {
  for (int x = 0; x < CELLS; x++) {
    for (int y = 0; y < MEMORY; y++) {
      fill(board[x][y]);
      rect(x * SCALE, y * SCALE, SCALE, SCALE);
    }
  }
}
