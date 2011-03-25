#define  LCD_ROWS   2
#define  LCD_COLS   8

void LCD_init(void);
void LCD_text(const char *buf);
void LCD_cursor(unsigned char line, unsigned char column);
void LCD_char(char c);
void LCD_hex(unsigned char value);
void LCD_specialchars(void);

