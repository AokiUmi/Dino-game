#include "lcd/lcd.h"
#include <string.h>
#include "utils.h"
#define X_MAX 158 /*0-159*/
#define Y_MAX 72 /*0-74*/

int option=0;
int state=0; /*diff state for two button*/


void Inp_init(void)
{
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

void Adc_init(void) 
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1);
    RCU_CFG0|=(0b10<<14)|(1<<28);
    rcu_periph_clock_enable(RCU_ADC0);
    ADC_CTL1(ADC0)|=ADC_CTL1_ADCON;
}

void IO_init(void)
{
    Inp_init(); // inport init
    Adc_init(); // A/D init
    Lcd_Init(); // LCD init
}
void jump()
{

}
void squat()
{

}
int judge_if_alive()
{

}
void play()
{

}
void settings()
{
    
}
void game_over()
{

}
void selection_start()
{
    delay_1ms(400);
    startmenu();
    choice(option,1);
    while(button()!=1)
    {
        if(state == 2)/*choose another option*/
        {
            choice(option,0);/*delete last option*/
            choice(change_option(),1);/*change option*/
            delay_1ms(300);
        }
    }
    if(option)
        settings();
    else 
        play();
}
int main(void)
{
    IO_init();         // init OLED
    // YOUR CODE HERE
    selection_start();//start our game

}
