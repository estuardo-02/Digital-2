/* 
 * File:   lcd_4bits.h
 * Author: estua
 *
 * Created on July 23, 2023, 7:53 PM
 */

#ifndef LCD_8BITS_H
#define	LCD_8BITS_H
//puertos en C
#define RS PORTEbits.RE1
#define EN PORTEbits.RE0
//resto de puerto en D

void Lcd_Port(char a)
{
	PORTD = a;
}
void Lcd_Cmd(char a)
{
	RS = 0;             // => RS = 0
	Lcd_Port(a);
	EN  = 1;             // => E = 1
        __delay_ms(4);
    EN  = 0;             // => E = 0
}

Lcd_Clear()
{
	Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b)
{
	char y_pos;
	if(a == 1) //posicion en x fijada
	{
        y_pos = 0x80 + b - 1;
		Lcd_Cmd(y_pos);
	}
	else if(a == 2)
	{
        y_pos = 0xC0 + b - 1;
		Lcd_Cmd(y_pos);
	}
}

void Lcd_Init()
{
  Lcd_Port(0x00);
   __delay_ms(100);
  Lcd_Port(0x38);
   __delay_ms(5); 
  Lcd_Port(0x38);
  __delay_ms(11); 
  Lcd_Port(0x38);

  //Fin de configuracion inciial, a partir de aqui BF esta activa
  Lcd_Cmd(0x0C); //visualizador apagado (reset)
  __delay_ms(5); 
  Lcd_Cmd(0x01);
 __delay_ms(50); 
  Lcd_Cmd(0x06);
  __delay_ms(5); 
}

void Lcd_Write_Char(char a)
{
   RS = 1;             // => RS = 1
   Lcd_Port(a);             //Data transfer
   EN = 1;
   __delay_us(40);
   EN  = 0;             // => E = 0
}

void Lcd_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	   Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right()
{
	Lcd_Cmd(0b00011100);
}

void Lcd_Shift_Left()
{
	Lcd_Cmd(0b00011000);
}



#endif	/* LCD_8BITS_H */

