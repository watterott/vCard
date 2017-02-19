/*
  vCard Demo App v2
  -----------------
  Select as Board: Arduino Pro (3.3V 8MHz) w/ ATmega328
*/


#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/* ---------- image data ----------*/
#define start_w (48)
byte start_data[] PROGMEM = 
{
0x00,0xE3,0xE3,0x81,0x89,0x89,0x89,0x89,0xF7,0xF7,0x00,0x00,0x00,0x87,0xC7,0xE1,0xA1,0xB1,0x99,0x89,0xCF,0xC7,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0xFF,0xFF,0x00,0x00,0x00,0x00,0x41,0x22,0x36,0x1C,0x3C,0xFF,0x3C,0x1C,0x1E,0x1A,0x11,0x10
};
#define pac_w (48)
byte pac_data[] PROGMEM = 
{
0x00,0x00,0x00,0x00,0x3C,0x7E,0xFF,0xFF,0xFD,0xEF,0x6E,0x2C,0x00,0x00,0x00,0x00,0x3C,0x7E,0xFF,0xFF,0xFD,0xE7,0x66,0x24,0x00,0x00,0x00,0x00,0x3C,0x7E,0xFF,0xFF,0xFD,0xE7,0x42,0x00,0x00,0x00,0x00,0x00,0x3C,0x7E,0xFF,0xFF,0xE5,0xC3,0x42,0x00
};
#define scroll_w (220)
byte scroll_data[] PROGMEM = 
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x08,0x08,0x08,0x08,0xFF,0x00,0x78,0x94,0x94,0x94,0x58,0x00,0x00,0xFF,0x00,0xFF,0x00,0x78,0x84,0x84,0x84,0x78,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x03,0x3C,0xC0,0x30,0x0E,0x01,0x0E,0x30,0xC0,0x3C,0x03,0x78,0x84,0x84,0x84,0x78,0x00,0xFC,0x08,0x04,0x00,0x00,0xFF,0x00,0x78,0x84,0x84,0x48,0xFF,0x00,0x00,0x00,0x00,0x00,0xBF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x41,0x22,0x36,0x1C,0x3C,0xFF,0x3C,0x1C,0x1E,0x1A,0x11,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x3C,0xC0,0x70,0x0E,0x01,0x0E,0x70,0xC0,0x3C,0x03,0x68,0x94,0x94,0x94,0xF8,0x00,0x04,0xFF,0x84,0x04,0xFF,0x84,0x00,0x78,0x94,0x94,0x94,0x58,0x00,0xFC,0x04,0x04,0x00,0x78,0x84,0x84,0x84,0x78,0x00,0x04,0xFF,0x84,0x04,0xFF,0x84,0x00,0x00,0x00,0x00,0x78,0x94,0x94,0x94,0x58,0x00,0xFF,0x00,0x78,0x94,0x94,0x94,0x58,0x00,0x78,0x84,0x84,0x84,0x48,0x00,0x04,0xFF,0x84,0x00,0xFC,0x04,0x04,0x00,0x78,0x84,0x84,0x84,0x78,0x00,0xFC,0x08,0x04,0x04,0xF8,0x00,0xFD,0x00,0x78,0x84,0x84,0x84,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x41,0x22,0x36,0x1C,0x3C,0xFF,0x3C,0x1C,0x1E,0x1A,0x11,0x10
};

/* ---------- led control data ----------*/
const byte led_port[12][2] = //0=PORTB, 1=PORTC
{
  {~(1<<0),    0xff},
  {~(1<<1),    0xff},
  {~(1<<2),    0xff},
  {~(1<<3),    0xff},
  {~(1<<4),    0xff},
  {~(1<<5),    0xff},
  {   0xff, ~(1<<0)},
  {   0xff, ~(1<<1)},
  {   0xff, ~(1<<2)},
  {   0xff, ~(1<<3)},
  {   0xff, ~(1<<4)},
  {   0xff, ~(1<<5)},
};
#define leds_off(x) { PORTB = 0xff;           PORTC = 0xff;           PORTD = 0x00; }
#define led_row(x)  { PORTB = led_port[x][0]; PORTC = led_port[x][1]; }

/* ---------- current frame ----------*/
byte frame[12];


#if   (F_CPU == 16000000)
# define TCNT_SUB 250 //1000Hz
# define COUNT      8 // 125Hz (every 8ms)
# define DELAY    500 //  80us (500us*12 = 6ms)
#elif (F_CPU ==  8000000)
# define TCNT_SUB 250 //500Hz
# define COUNT      4 //125Hz (every 8ms)
# define DELAY    500 //500us (500us*12 = 6ms)
#else
# warning clock not supported
#endif
ISR(TIMER2_OVF_vect) //Timer 2 Interrupt
{
  static uint8_t count=1;
  uint8_t x;

  TCNT2 -= TCNT_SUB;

  if(--count == 0)
  {
    count = COUNT;

    //draw frame
    for(x=0; x<12; x++)
    {
      PORTD = frame[x];
      led_row(x);
      _delay_us(DELAY);
    }
    leds_off();
  }
}


void set_pixel(uint8_t nr) //set pixel in frame
{
  frame[(nr>>3)] |= (1<<(nr&7));
}


void clr_pixel(uint8_t nr) //clear pixel in frame
{
  frame[(nr>>3)] &= ~(1<<(nr&7));
}


void set_row(uint8_t r, uint8_t d) //set row in frame
{
  if(r < 12)
  {
    frame[r] = d;
  }
}


void clr_row(uint8_t r) //clear row in frame
{
  if(r < 12)
  {
    frame[r] = 0x00;
  }
}


void set_frame(PGM_P s) //set complete frame (s = pointer to flash start addr)
{
  frame[ 0] = pgm_read_byte(s++);
  frame[ 1] = pgm_read_byte(s++);
  frame[ 2] = pgm_read_byte(s++);
  frame[ 3] = pgm_read_byte(s++);
  frame[ 4] = pgm_read_byte(s++);
  frame[ 5] = pgm_read_byte(s++);
  frame[ 6] = pgm_read_byte(s++);
  frame[ 7] = pgm_read_byte(s++);
  frame[ 8] = pgm_read_byte(s++);
  frame[ 9] = pgm_read_byte(s++);
  frame[10] = pgm_read_byte(s++);
  frame[11] = pgm_read_byte(s);
}


void set_frame_inv(PGM_P s) //set complete frame inverted (s = pointer to flash start addr)
{
  frame[ 0] = ~pgm_read_byte(s++);
  frame[ 1] = ~pgm_read_byte(s++);
  frame[ 2] = ~pgm_read_byte(s++);
  frame[ 3] = ~pgm_read_byte(s++);
  frame[ 4] = ~pgm_read_byte(s++);
  frame[ 5] = ~pgm_read_byte(s++);
  frame[ 6] = ~pgm_read_byte(s++);
  frame[ 7] = ~pgm_read_byte(s++);
  frame[ 8] = ~pgm_read_byte(s++);
  frame[ 9] = ~pgm_read_byte(s++);
  frame[10] = ~pgm_read_byte(s++);
  frame[11] = ~pgm_read_byte(s);
}


void clr_frame(void) //clear complete frame
{
  frame[0] = frame[1] = frame[2]  = frame[3]  =
  frame[4] = frame[5] = frame[6]  = frame[7]  =
  frame[8] = frame[9] = frame[10] = frame[11] = 0x00;
}


uint8_t running_light(void)
{
  static uint8_t dir=0, pos=0;

  if(dir & 0x01)
  {
    pos--;
  }
  else
  {
    pos++;
  }

  clr_frame();
  set_row(pos, 0xff);

  if((pos == 0) || (pos >= 11))
  {
    dir++;
    if(dir >= 4)
    {
      dir = 0;
      pos = 0;
      delay(90);
      clr_frame();
      return 1;
    }
  }

  delay(90);

  return 0;
}


uint8_t start_countdown(void)
{
  static uint8_t pos=0;

  set_frame((PGM_P)&start_data[pos]);

  if((pos+12) >= start_w)
  {
    pos = 0;
    delay(1000);
    return 1;
  }
  else
  {
    pos += 12;
  }

  delay(1000);

  return 0;
}


uint8_t pacman(void)
{
  static uint16_t pos=0, offset=0;
  uint16_t i, j;

  i = pos;
  j = 12;
  do
  {
    if(j)
    {
      set_row(i, pgm_read_byte((PGM_P)&pac_data[offset+j-1]));
      j--;
    }
    else
    {
      clr_row(i);
    }
    i--;
  }while(i!=(-1));
  delay(50-pos);
  
  offset += 12;
  if(offset >= pac_w)
  {
    offset = 0;
    if(pos >= 23)
    {
      pos = 0;
      delay(500);
      clr_frame();
      return 1;
    }
    else
    {
      pos += 1;
    }
  }

  return 0;
}


uint8_t scroll_text(void)
{
  static uint16_t pos=0;

  set_frame((PGM_P)&scroll_data[pos]);

  if((pos+12) >= scroll_w)
  {
    pos = 0;
    delay(3000);
    return 1;
  }
  else
  {
    pos += 1;
    delay(90);
  }

  return 0;
}


uint8_t gol_alive(uint8_t x, uint8_t y)
{
  if((x < 12) && (y < 8))
  {
    return frame[x] & (1<<y);
  }

  return 0;
}


uint8_t gol_neighbors(uint8_t x, uint8_t y)
{
  uint8_t count=0;

  //3 above
  if(gol_alive(x-1,y-1)){ count++; }
  if(gol_alive(x,  y-1)){ count++; }
  if(gol_alive(x+1,y-1)){ count++; }

  //2 on each side
  if(gol_alive(x-1,  y)){ count++; }
  if(gol_alive(x+1,  y)){ count++; }

  //3 below
  if(gol_alive(x-1,y+1)){ count++; }
  if(gol_alive(  x,y+1)){ count++; }
  if(gol_alive(x+1,y+1)){ count++; }

  return count;
}


uint8_t gol(void)
{
  static uint8_t generation=0;
  uint8_t x, y, count;
  byte buf[12];

  //update cells
  for(x=0; x < 12; x++)
  {
    buf[x] = frame[x];
    for(y=0; y < 8; y++)
    {
      count = gol_neighbors(x, y);
      if((count == 3) && !gol_alive(x, y))
      {
        buf[x] |= (1<<y); //new cell
      }
      if(((count < 2) || (count > 3)) && gol_alive(x, y))
      {
        buf[x] &= ~(1<<y); //cell dies
      }
    }
  }

  //copy buf to frame
  for(count=0; count < 12; count++)
  {
    frame[count] = buf[count];
  }

  delay(150);

  if(++generation > 60)
  {
    generation = 0;
    return 1;
  }

  return 0;
}


void setup()
{
  byte c;

  //check if manual reset, then pins -> input with pull-up
  c = MCUSR;
  if(c & (1<<EXTRF))
  {
    MCUSR = 0;
    PORTB = 0xFF;
    DDRB  = 0x00;
    PORTC = 0xFF;
    DDRC  = 0x00;
    PORTD = 0xFF;
    DDRD  = 0x00;
    delay(300);
  }

  //pins -> output
  DDRB  = 0xFF;
  DDRC  = 0xFF;
  DDRD  = 0xFF;

  leds_off();
  clr_frame();

  //init Timer2
  TCCR2B  = (1<<CS22); //clk=F_CPU/64
  TCNT2   = 0x00;
  TIMSK2 |= (1<<TOIE2); //enable overflow interupt

  //interrupts on
  sei();
}


void loop()
{
  static uint8_t state=0;

  switch(state)
  {
    case 0: //--- running light ---
      state += running_light();
      break;

    case 1: //--- start count down ---
      state += start_countdown();
      break;

    case 2: //--- pacman ---
      state += pacman();
      break;

    case 3: //--- scroll text ---
      state += scroll_text();
      break;

    case 4: //--- game of live ---
      state++;
      //make better content for game of live
      for(uint8_t i=0; i<12; i++)
      {
        frame[i] &= 0xE7;
      }
    case 5:
      if(gol())
      {
        state = 2;
      }
      break;
  }
}
