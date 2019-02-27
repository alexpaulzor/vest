#include <Adafruit_NeoPixel.h>

#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )
#define arr2_len( x )  ( arr_len( x ) * arr_len( x[0] ) )

#define FRONT_PIN 6
#define FRONT_LEDS 30
#define BACK_LEDS 34
#define ROWS 5
#define COLUMNS 9

#define BACK_PIN 8

#define ANIMATION_DELAY 150 // milliseconds

// #define DEBUG false

/*
 Pixel locations

Front:
 0                       8
 1   2   3   4   5   6   7
 15  14  13  12  11  10  9
 16  17  18  19  20  21  22
 29  28  27  26  25  24  23
 
 
 Back:
      0                       8
      1   2   3   4   5   6   7
      15  14  13  12  11  10  9
  16  17  18  19  20  21  22  23  24
  33  32  31  30  29  28  27  26  25
*/
/*
short LED_ROWS[ROWS][COLUMNS] = {
  { 0, NULL, NULL, NULL, NULL, NULL, 8 },
  { 1,  2,  3,  4,  5,  6,  7  },
  { 15, 14, 13, 12, 11, 10, 9  },
  { 16, 17, 18, 19, 20, 21, 22 },
  { 29, 28, 27, 26, 25, 24, 23 }
}; // */

short FRONT_COLUMNS[COLUMNS][ROWS] = {
  { 255, 255, 255, 255, 255 },
  { 0,   1,   15,  16,  29 },
  { 255, 2,   14,  17,  28 },
  { 255, 3,   13,  18,  27 },
  { 255, 4,   12,  19,  26 },
  { 255, 5,   11,  20,  25 },
  { 255, 6,   10,  21,  24 },
  { 8,   7,   9,   22,  23 },
  { 255, 255, 255, 255, 255 }
}; // */

short BACK_COLUMNS[COLUMNS][ROWS] = {
  { 255, 255, 255, 16, 33 },
  { 0,   1,   15,  17, 32 },
  { 255, 2,   14,  18, 31 },
  { 255, 3,   13,  19, 30 },
  { 255, 4,   12,  20, 29 },
  { 255, 5,   11,  21, 28 },
  { 255, 6,   10,  22, 27 },
  { 8,   7,   9,   23, 26 },
  { 255, 255, 255, 24, 25 }
}; // */

short get_front_led(short x, short y) {
  if (x < 0 || y < 0 || x >= COLUMNS || y >= ROWS) return 255;
  return FRONT_COLUMNS[x][y];
}

short get_back_led(short x, short y) {
  if (x < 0 || y < 0 || x >= COLUMNS || y >= ROWS) return 255;
  return BACK_COLUMNS[x][y];
}

Adafruit_NeoPixel front = Adafruit_NeoPixel(FRONT_LEDS, FRONT_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel back = Adafruit_NeoPixel(BACK_LEDS, BACK_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  randomSeed(analogRead(0));
  front.begin();
  back.begin();
  //front.setBrightness(BRIGHTNESS);
  //back.setBrightness(BRIGHTNESS);
  front.show();
  back.show();
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
}

void loop() {
  fade_in_out();
  
  line_sweep();
  line_sweep();
  boxes();
  boxes();
  spiral();
  spiral();
  
  fade_in_out();
  
  line_sweep();
  line_sweep();
  boxes();
  boxes();
  spiral();
  spiral();
  
  fade_in_out();

  equalizer();
  
  fade_in_out();

  //pong();

  spaz(); //*/
}

void set_strip_pixel(Adafruit_NeoPixel & strip, short pixel, short red, short green, short blue) {
  #ifdef DEBUG
  String msg;
  msg = String("Setting pixel ");
  msg = msg + (int)pixel + " to (" + (int)red + ", " + (int)green + ", " + (int)blue + ")";
  Serial.println(msg); // */
  #endif
  if (pixel >= strip.numPixels()) {
    return;
  }
  strip.setPixelColor(pixel, red, green, blue);
}
/*
void set_pixel(short pixel, short red, short green, short blue) {
  set_strip_pixel(front, pixel, red, green, blue);
  set_strip_pixel(back, pixel, red, green, blue);
}
*/
void set_pixel(short x, short y, short red, short green, short blue) {
  set_strip_pixel(front, get_front_led(x, y), red, green, blue);
  set_strip_pixel(back, get_back_led(x, y), red, green, blue);
}

void reset() {
  set_all(0, 0, 0);
}

void set_all(short red, short green, short blue) {
  set_strip_all(front, red, green, blue);
  set_strip_all(back, red, green, blue);
}

void set_strip_all(Adafruit_NeoPixel & strip, short red, short green, short blue) { 
  for (short i = 0; i < strip.numPixels(); i++) {
    set_strip_pixel(strip, i, red, green, blue);
  }
}

void flush() {
  flush(1);
}

void flush(short ticks) {
  front.show();
  back.show();
  delay(ticks * ANIMATION_DELAY);
}

short randcolor() {
  return random(5) * 64 % 256;
}

void spaz() {
  reset();
  flush();
  for (short i = 0; i < 30; i++) {
    reset();
    set_strip_pixel(front, random(front.numPixels()), randcolor(), randcolor(), randcolor());
    set_strip_pixel(front, random(front.numPixels()), randcolor(), randcolor(), randcolor());
    set_strip_pixel(back, random(back.numPixels()), randcolor(), randcolor(), randcolor());
    set_strip_pixel(back, random(back.numPixels()), randcolor(), randcolor(), randcolor());

    flush();
  }
}

void equalizer() {
  reset();
  flush();
  // columns 0 - 6, level 0 - 4 (upside down)
  short levels[COLUMNS] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  
  // layers 3, 4 = green
  // layer 2 = yellow
  // layers 1, 0 = red
  short row_colors[][3] = {
    { 255, 0, 0 },
    { 255, 0, 0 },
    { 255, 255, 0 },
    { 0, 255, 0 },
    { 0, 255, 0 }
  };
  
  for (short i = 0; i < 50; i++) { // run for 5 seconds
    reset();
    for (short col = 0; col < COLUMNS; col++) {
      
      levels[col] = levels[col] + random(4) - 2;
      if (i % 5 == 0) {
        levels[col] += 1;
        if (col == 1) levels[col] += 1;
      }
      levels[col] = levels[col] < 0 ? 0 : (levels[col] > 4 ? 4 : levels[col]);

      for (short row = 4 - levels[col]; row <= 4; row++) {
        set_pixel(col, row, row_colors[row][0], row_colors[row][1], row_colors[row][2]);
      }
    }
    flush();
  }
}

void fade_in_out() {
  reset();
  flush();
  short red = randcolor();
  short green = randcolor();
  short blue = randcolor();
  //TODO: change increment to control smoothness/speed
  for (short i = 0; i <= 255; i += 8) {
    set_all(red * i / 255,
      green * i / 255,
      blue * i / 255);
      
    flush();
  }
  //reset();
  flush();
  for (short i = 255; i >= 0; i -= 8) {
    set_all(red * i / 255,
      green * i / 255,
      blue * i / 255);
      
    flush();
  }
}


void spiral() {
  reset();
  flush();
  
  short steps[FRONT_LEDS][2] = { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 },
    { 1, 4 }, { 2, 4 }, {3, 4}, {4, 4}, {5, 4}, {6, 4},
    {6, 3}, {6, 2}, {6, 1}, {6, 0},
    {1, 1}, { 1, 2 }, { 1, 3 },
    {2, 3 }, {3, 3}, {4, 3}, {5, 3},
    {5, 2}, {5, 1},
    {4, 1}, {3, 1}, {2, 1},
    {2, 2}, {3, 2}, {4, 2}  };


  short red = randcolor();
  short green = randcolor();
  short blue = randcolor();

  for (short i = 0; i < FRONT_LEDS; i++) {
    set_pixel(steps[i][0] + 1, steps[i][1], red, green, blue);
    flush();
  }
  for (short i = 0; i < FRONT_LEDS; i++) {
    set_pixel(steps[i][0] + 1, steps[i][1], 0, 0, 0);
    flush();
  }

  flush();
} // */

// Sweep columns left to right, and then rows top to bottom
void line_sweep() {
  short red = randcolor();
  short green = randcolor();
  short blue = randcolor();
  reset();
  flush();
  for (short column = 0; column < COLUMNS; column++) {
    for (short row = 0; row < ROWS; row++) {
      set_pixel(column, row, red, green, blue);
    }
    flush();
    for (short row = 0; row < ROWS; row++) {
      set_pixel(column, row, 0, 0, 0);
    }
  }
  flush();

  for (short row = 0; row < ROWS; row++) {
    for (short column = 0; column < COLUMNS; column++) {
      set_pixel(column, row, red, green, blue);
    }
    flush();
    for (short column = 0; column < COLUMNS; column++) {
      set_pixel(column, row, 0, 0, 0);
    }
  }
  flush();

  // put my thing down flip it and reverse it

  for (short column = COLUMNS - 1; column >= 0; column--) {
    for (short row = 0; row < ROWS; row++) {
      set_pixel(column, row, red, green, blue);
    }
    flush();
    for (short row = 0; row < ROWS; row++) {
      set_pixel(column, row, 0, 0, 0);
    }
  }
  flush();

  for (short row = ROWS - 1; row >= 0; row--) {
    for (short column = 0; column < COLUMNS; column++) {
      set_pixel(column, row, red, green, blue);
    }
    flush();
    for (short column = 0; column < COLUMNS; column++) {
      set_pixel(column, row, 0, 0, 0);
    }
  }
  flush();
}

void boxes() {
  short red = randcolor();
  short green = randcolor();
  short blue = randcolor();
  reset();
  flush();

  // bottom right -> top left
  //
  short y = ROWS - 1; 
  for (short x = COLUMNS - 2; x >= 1; x--) {
     // draw a box 
    reset();
    for (short col = x; col < COLUMNS - 1; col++) {
      set_pixel(col, y, red, green, blue);
      set_pixel(col, ROWS - 1, red, green, blue);
    }
    for (short row = y; row < ROWS; row++) {
      set_pixel(x, row, red, green, blue);
      set_pixel(COLUMNS - 2, row, red, green, blue);
    }
    flush();
    
    if (y > 1) {
      y--;
    }
  }

  flush(2);

  y = ROWS - 1; 
  // shrink the box
  for (short x = COLUMNS - 2; x >= 1; x--) {
     // draw a box 
    reset();
    for (short col = 1; col <= x; col++) {
      set_pixel(col, y, red, green, blue);
      set_pixel(col, 1, red, green, blue);
    }
    for (short row = 1; row <= y; row++) {
      set_pixel(x, row, red, green, blue);
      set_pixel(1, row, red, green, blue);
    }
    flush();
    
    if (y > 1 && x < ROWS) { // TODO: was ROWS - 1?
      y--;
    }
  }

  reset();
  flush();

  red = randcolor();
  green = randcolor();
  blue = randcolor();
 
  // top right -> bottom left
  //
  y = 1; 
  for (short x = COLUMNS - 2; x >= 1; x--) {
     // draw a box 
    reset();
    for (short col = x; col < COLUMNS - 1; col++) {
      set_pixel(col, y, red, green, blue);
      set_pixel(col, 1, red, green, blue);
    }
    for (short row = 1; row <= y; row++) {
      set_pixel(x, row, red, green, blue);
      set_pixel(COLUMNS - 2, row, red, green, blue);
    }
    flush();
    
    if (y < ROWS - 1) {
      y++;
    }
  }

  flush(2);

  y = 1; 
  // shrink the box
  for (short x = COLUMNS - 2; x >= 1; x--) {
     // draw a box 
    reset();
    for (short col = 1; col <= x; col++) {
      set_pixel(col, y, red, green, blue);
      set_pixel(col, ROWS - 1, red, green, blue);
    }
    for (short row = y; row < ROWS; row++) {
      set_pixel(x, row, red, green, blue);
      set_pixel(1, row, red, green, blue);
    }
    flush();
    
    if (y < ROWS - 1 && x < ROWS) { // TODO: was ROWS - 1?
      y++;
    }
  }

  reset();
  flush();
}
/*
void pong() {
  reset();
  flush();

  short x = 4;
  short y = 2;

  short left_y = 2;
  short right_y = 2;

  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++;
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++;
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++; //5
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++; //6
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 5
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 4
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 3
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 2
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 1
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 0
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++; // 1
  y++; // 3
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++; // 2
  y++; // 4
  right_y++; // 3
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++; // 3
  y--; // 3
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++; // 4
  y--; // 2
  right_y--; // 2
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++; // 5
  y--; // 1
  right_y--; // 2
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++; // 6
  y++; // 2
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 5
  y++; // 3
  left_y++; // 3
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 4
  y++; // 4
  left_y++; //4
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 3
  y--; // 3
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 2
  y--; // 2
  left_y--; // 3
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 1
  y--; // 1
  left_y--; // 2
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x--; // 0
  y++; // 2
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  x++; // 1 (but really 2)
  y++; // 3
  pong_board(x, y, left_y, right_y, 0, 0, 255);
  x++; //2
  y++; //4
  pong_board(x, y, left_y, right_y, 0, 0, 255);
  x++; //3
  y--; //3
  pong_board(x, y, left_y, right_y, 0, 0, 255);
  x++; //4
  y--; //2
  pong_board(x, y, left_y, right_y, 0, 0, 255);
  x++; //5
  y--; //1
  pong_board(x, y, left_y, right_y, 0, 0, 255);
  x++; // 6
  y++; // 2
  pong_board(x, y, left_y, right_y, 0, 0, 255);
  
  x++; //7
  y++; //3
  
  pong_board(x, y, left_y, right_y, 0, 0, 255);
  pong_board(x, y, left_y, right_y, 255, 0, 0);
  pong_board(x, y, left_y, right_y, 0, 255, 0);
  pong_board(x, y, left_y, right_y, 0, 0, 255);
  pong_board(x, y, left_y, right_y, 255, 0, 0);
}

void pong_board(short x, short y, short left_y, short right_y, short red, short green, short blue) {
  reset();
  set_pixel(1, left_y, 255, 255, 255);
  set_pixel(COLUMNS - 2, right_y, 255, 255, 255);
  set_pixel(x, y, red, green, blue);
  flush();
} */
