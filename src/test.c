#include "lcd/lcd.h"
#include <string.h>
#include <time.h>
#include "utils.h"
#include "img.h"
#define U16_MAX 65535
#define X_MAX 160 /*0-159*/
#define Y_MAX 80  /*0-79*/
// some basic constants
#define JUMP_MAX_HEIGHT 35
#define CACTUS_MAX_HEIGHT 20
#define GROUND_HEIGHT 10
#define PTER_HEIGHT 20
#define TREX_WLAK_HEIGHT 20
#define TREX_SQUAT_HEIGHT 10
#define OBSTACLE_START_POS 120
#define INITIAL_GROUND1_POS 0
#define INITIAL_GROUND2_POS 160
int getBoot0 = 0;                          // bool for boot0
int getButton1 = 0;                        // bool for button1
int getButton0 = 0;                        // bool for button0
u16 game_run_time = 0;                     // run time when game start = score
u16 obstacle_interval, obstacle_last_time; // time for interval and memorize when the last obstacle appear
int judge_if_alive = 1;                    // when update image, compute if trex alive
uint32_t diff =35;                       // decide the delay in main while-loop
int count;                                 // use it to calculate how many times the loop run
unsigned int timer = 0; // used to generate random u16


enum Game_Status
{
    Home_page = 0,
    Play_page,
    Settings_page,
    End_page // game status
};
enum Trex_Status
{
    Walk_1 = 0,
    Walk_2,
    Jump,
    Squat_1,
    Squat_2 // trex status
};
enum Obstacle
{
    Cactus_1 = 0,
    Cactus_2,
    Pter_1,
    Pter_2 // obstacle status
};

typedef struct Trex_Information_
{
    enum Trex_Status trex_status; // represent trew's status
    int jump_height;              // if trex_status=Jump, memorize now height
    int if_rise;                  //  if_rise==1 ,trex_status=Jump and it is rise now or it is down
} Trex_Information;
typedef struct Obstacle_Information_
{
    int x;
    enum Obstacle object_status;
} Obstacle_Information;
typedef struct Game_Information_
{
    Trex_Information *Trex;
    enum Game_Status state;
    int obstacle_number;
    Obstacle_Information **obstacle;
    int ground1_pos; // two ground pic using it both
    int ground2_pos;
} Game_Information;

struct Picture
{
    int width, height;
    uint8_t *array;
};
void Inp_init(void)
{
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

void Adc_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
    RCU_CFG0 |= (0b10 << 14) | (1 << 28);
    rcu_periph_clock_enable(RCU_ADC0);
    ADC_CTL1(ADC0) |= ADC_CTL1_ADCON;
}

void IO_init(void)
{
    Inp_init(); // inport init
    Adc_init(); // A/D init
    Lcd_Init(); // LCD init
}
Game_Information *Game_init()
{
    Game_Information *new_game = (Game_Information *)malloc(sizeof(Game_Information));
    new_game->Trex = (Trex_Information *)malloc(sizeof(Trex_Information));
    new_game->Trex->trex_status = Walk_1;
    new_game->Trex->jump_height = 0;
    new_game->Trex->if_rise = 0;
    new_game->obstacle_number = 0;
    new_game->obstacle = NULL;
    new_game->state = Home_page;
    new_game->ground1_pos = INITIAL_GROUND1_POS;
    new_game->ground2_pos = INITIAL_GROUND2_POS;
    return new_game;
}
void delete_game(Game_Information * game)
{
    for(int i=0;i<game->obstacle_number;i++)
        free(game->obstacle[i]);
    free(game->obstacle);
    free(game->Trex);
    free(game);
    game=NULL;
}
/// Step 1: input
Game_Information * reinit_game(Game_Information * game)
{
	delete_game(game);
	Game_Information * new_game=Game_init();
	return new_game;

}
void ProcessInput()
{
    getBoot0 = Get_BOOT0();
    getButton0 = Get_Button(0);
    getButton1 = Get_Button(1);
}

/// Step 2: logic define



void WriteToImageBuffer(bool  *image, unsigned char  *object, int width, int height,int x0, int y0) // write each little object to the whole screen
{
	for(int x=0;x<width*2;x++)
		for(int y=0;y<height;y++)
	{
		if((y0+y)>=Y_MAX||x0+x<0||x0+x>=X_MAX*2||y0+y<0)continue;
		if(image[(y0+y)*X_MAX*2+x0]==TRUE)
			judge_if_alive=0;
        if(object[y*width*2+x]==0xff)
		    image[(y0+y)*X_MAX*2+x0]=TRUE;

	
	}
    return;
}
void init_image(bool *image) // play start time=0
{
	WriteToImageBuffer(image,g1,160,GROUND_HEIGHT,0,64);
	WriteToImageBuffer(image,trex1,20,TREX_WLAK_HEIGHT,25,44);
	WriteToImageBuffer(image,cactus1,12,CACTUS_MAX_HEIGHT,120,44);
    /*
    LCD_ShowPic(120,44,131,63,cactus1,sizeof(cactus1)/sizeof(unsigned char));	
    LCD_ShowPic(0,64,159,73,g1,sizeof(g1)/sizeof(unsigned char));
    LCD_ShowPic(25,44,51,63,trex5,sizeof(trex5)/sizeof(unsigned char));
    LCD_ShowPic(25,27,44,46,pter2,sizeof(pter2)/sizeof(unsigned char));
    LCD_ShowPic(100,44,111,63,cactus2,sizeof(cactus2)/sizeof(unsigned char));	*/
   
}
void Draw() // use LCD_drawpic
{
   LCD_ShowPic(0,64,159,73,g1,sizeof(g1)/sizeof(unsigned char));
   LCD_ShowPic(25,44,44,63,trex1,sizeof(trex1)/sizeof(unsigned char));
}
unsigned char ground[10*160*2];
void update_ground(Game_Information *game)
{
    game->ground1_pos--;game->ground2_pos--;
    if(game->ground1_pos==-160)
    {
       	game->ground1_pos = INITIAL_GROUND1_POS;
    	game->ground2_pos = INITIAL_GROUND2_POS;
    }
    for(int x=0;x<160*2;x++)
		for(int y=0;y<10;y++)
	{
		if(x+game->ground1_pos>=0 && x+game->ground1_pos< 160*2)
		ground[y*160*2+x+game->ground1_pos]=g1[y*160*2+x];
		if(x+game->ground2_pos>=0 && x+game->ground2_pos< 160*2)
		ground[y*160*2+x+game->ground2_pos]=g1[y*160*2+x];
	}
    LCD_ShowPic(0,64,159,73,ground,sizeof(ground)/sizeof(unsigned char));
    
}

void reorder(Game_Information *game)
{
    for(int i=1 ;i<game->obstacle_number ;i++)// 1->0 2->1
    {
    	LCD_DeletePic(0,23,19,63);
        game->obstacle[i-1]->object_status=game->obstacle[i]->object_status;
        game->obstacle[i-1]->x=game->obstacle[i]->x;
    }
	return;
}
void update_obstracle(Game_Information *game)
{
 	if(game->obstacle[0]->x==0)//delete obstacle
    {
        reorder(game); // 1->0 2->1
        game->obstacle_number--;
        game->obstacle=(Obstacle_Information **)realloc(game->obstacle,game->obstacle_number*sizeof(Obstacle_Information *));
    }
    for(int i=0;i< game->obstacle_number;i++)//different obstacle
    {
        game->obstacle[i]->x--;
        if(game->obstacle[i]->object_status==Cactus_1)
        {
        	LCD_DeletePic(game->obstacle[i]->x+1,44,game->obstacle[i]->x+12,63);	
            LCD_ShowPic(game->obstacle[i]->x,44,game->obstacle[i]->x+11,63,cactus1,sizeof(cactus1)/sizeof(unsigned char));	
        }
        else if(game->obstacle[i]->object_status==Cactus_2)
        {
        	LCD_DeletePic(game->obstacle[i]->x+1,44,game->obstacle[i]->x+12,63);	
            LCD_ShowPic(game->obstacle[i]->x,44,game->obstacle[i]->x+11,63,cactus2,sizeof(cactus2)/sizeof(unsigned char));	
        }
        else if(game->obstacle[i]->object_status==Pter_1)
        {
            LCD_DeletePic(game->obstacle[i]->x+1,26,game->obstacle[i]->x+20,45);	
            LCD_ShowPic(game->obstacle[i]->x,26,game->obstacle[i]->x+19,45,pter2,sizeof(pter2)/sizeof(unsigned char));
            game->obstacle[i]->object_status=Pter_2;
        }
        else if(game->obstacle[i]->object_status==Pter_2)
        {
            LCD_DeletePic(game->obstacle[i]->x+1,26,game->obstacle[i]->x+20,45);	
            LCD_ShowPic(game->obstacle[i]->x,26,game->obstacle[i]->x+19,45,pter1,sizeof(pter1)/sizeof(unsigned char));
            game->obstacle[i]->object_status=Pter_1;
        }
    }
    if(game_run_time-obstacle_last_time==obstacle_interval) // you should update new obstacle
    {
        obstacle_last_time=game_run_time;
        obstacle_interval=Generate_U16(75,100,timer);
        int x=Generate_U16(0,10000,timer);
        if(x%3==0)
        {
            LCD_ShowPic(OBSTACLE_START_POS,44,OBSTACLE_START_POS+11,63,cactus1,sizeof(cactus1)/sizeof(unsigned char));	
            game->obstacle_number++;
            game->obstacle=(Obstacle_Information **)realloc(game->obstacle,game->obstacle_number*sizeof(Obstacle_Information *));
            game->obstacle[game->obstacle_number-1]=(Obstacle_Information *)malloc(sizeof(Obstacle_Information));
            game->obstacle[game->obstacle_number-1]->x=OBSTACLE_START_POS;
            game->obstacle[game->obstacle_number-1]->object_status=Cactus_1;
        }
        else if(x%3==1) 
        {
            LCD_ShowPic(OBSTACLE_START_POS,44,OBSTACLE_START_POS+11,63,cactus2,sizeof(cactus2)/sizeof(unsigned char));	
            game->obstacle_number++;
            game->obstacle=(Obstacle_Information **)realloc(game->obstacle,game->obstacle_number*sizeof(Obstacle_Information *));
            game->obstacle[game->obstacle_number-1]=(Obstacle_Information *)malloc(sizeof(Obstacle_Information));
            game->obstacle[game->obstacle_number-1]->x=OBSTACLE_START_POS;
            game->obstacle[game->obstacle_number-1]->object_status=Cactus_2;
        }
        else 
        {
            LCD_ShowPic(OBSTACLE_START_POS,26,OBSTACLE_START_POS+19,45,pter1,sizeof(pter1)/sizeof(unsigned char));	
            game->obstacle_number++;
            game->obstacle=(Obstacle_Information **)realloc(game->obstacle,game->obstacle_number*sizeof(Obstacle_Information *));
            game->obstacle[game->obstacle_number-1]=(Obstacle_Information *)malloc(sizeof(Obstacle_Information));
            game->obstacle[game->obstacle_number-1]->x=OBSTACLE_START_POS;
            game->obstacle[game->obstacle_number-1]->object_status=Pter_1;
        }
    }
}

u16 bit_num(u16 x)// calculate the bit of score
{
    u16 ans=0;
    while(x)
    {
        x=x/10;
        ans++;
    }
    return ans;
}
void show_score(u16 score)
{
	LCD_ShowNum(125,10,score,bit_num(score),0xFFFE);
}
int Home_choice()
{

    int make_choice = 0, now_choice = -1;
    while (TRUE)
    {
        if (make_choice == 1 && Get_BOOT0() == 1) // identify your choice
            break;
        if ( (( now_choice == -1 && getButton0 == 1 )|| Get_Button(0) == 1 )&& now_choice != 0) // choose easy/retry
        {
            if(now_choice != -1)
            	choice_start(now_choice, 0);
            choice_start(0, 1);
            make_choice = 1;
            now_choice = 0;
        }
        else if ((( now_choice == -1 && getButton1 == 1 ) || Get_Button(1) == 1 ) && now_choice != 1) // choose hard/quit
        {
            if(now_choice != -1)
            	choice_start(now_choice, 0);
            choice_start(1, 1);
            make_choice = 1;
            now_choice = 1;
        }
    }
    return now_choice;
}
int make_choice()
{

    int make_choice = 0, now_choice =-1;

    while (TRUE)
    {
        if (make_choice == 1 && Get_BOOT0() == 1) // identify your choice
            break;
        if ( (( now_choice == -1 && getButton0 == 1 )|| Get_Button(0) == 1 )&& now_choice != 0) // choose easy/retry
        {
            if(now_choice != -1)
            	choice_select(now_choice, 0);
            choice_select(0, 1);
            make_choice = 1;
            now_choice = 0;
        }
        else if ((( now_choice == -1 && getButton1 == 1 ) || Get_Button(1) == 1 ) && now_choice != 1) // choose hard/quit
        {
            if(now_choice != -1)
            	choice_select(now_choice, 0);
            choice_select(1, 1);
            make_choice = 1;
            now_choice = 1;
        }
    }
    return now_choice;
}
void judge_trex_alive(Game_Information *game)
{
	if(game->obstacle_number==0)return;
    if((game->Trex->trex_status==Walk_1 || game->Trex->trex_status==Walk_2 || game->Trex->trex_status==Jump)&& game->obstacle[0]->x>44)
        return;
    if((game->Trex->trex_status==Squat_1 || game->Trex->trex_status==Squat_2) && game->obstacle[0]->x>51)
    	return;
    bool  **image; // use *image to judge 52*2 *64
	image=(bool **)malloc((52*2)*sizeof(bool *));
	for(int i=0 ;i< 52*2 ;i++)
	{
		image[i]=(bool *)malloc(sizeof(bool)*64);
		for(int j=0;j<64;j++)	
			image[i][j]=FALSE;
	}
    	
    if(game->Trex->trex_status==Walk_1 || game->Trex->trex_status==Walk_2 || game->Trex->trex_status==Jump )//Initial trex pos
    {
        int x0=25,y0;
        if(game->Trex->trex_status==Jump)   
            y0=44-game->Trex->jump_height;
        else y0=44;
        for(int x=0;x<20*2;x++)
            for(int y=0;y<20;y++)
            {
                if(game->Trex->trex_status==Walk_1&&trex1[y*20*2+x]==0xff)
                    image[x+x0][y+y0]=TRUE;
                else if(game->Trex->trex_status==Walk_2&&trex2[y*20*2+x]==0xff)
                    image[x+x0][y+y0]=TRUE;
                else if(game->Trex->trex_status==Jump&&trex3[y*20*2+x]==0xff)
                    image[x+x0][y+y0]=TRUE;
            }
                
            
    }
    else // squat
    {
        int x0=25,y0=44;
        for(int x=0;x<27*2;x++)
            for(int y=0;y<20;y++)
            {
                if(trex4[y*27*2+x]==0xff) 
                    image[x+x0][y+y0]=TRUE;
            }
               
    }
    if(game->obstacle[0]->object_status==Cactus_1 || game->obstacle[0]->object_status==Cactus_2) //cactus1 or cactus2
    {
        int x0=game->obstacle[0]->x;
        int y0=44;
        for(int x=0;x<12*2;x++)
            for(int y=0;y<20;y++)
            {
                if(x+x0>=0 && x+x0 < 52*2 && y+y0>=0 && y+y0 < 64)
                    if(image[x+x0][y+y0]==TRUE)
                    {
                        if(game->obstacle[0]->object_status==Cactus_1 &&cactus1[y*12*2+x]==0xff )
                        {
                            judge_if_alive=0;
                            break;
                        }
                        else if(game->obstacle[0]->object_status==Cactus_2 &&cactus2[y*12*2+x]==0xff )
                        {
                            judge_if_alive=0;
                            break;
                        }
                    }
                        
            }
    }
    else //pter1 or pter2
    {
        int x0=game->obstacle[0]->x;
        int y0=26;
        for(int x=0;x<20*2;x++)
            for(int y=0;y<20;y++)
            {
                if(x+x0>=0 && x+x0 < 52*2 && y+y0>=0 && y+y0 < 64)
                    if(image[x+x0][y+y0]==TRUE)
                    {
                        if(game->obstacle[0]->object_status==Pter_1 &&pter1[y*20*2+x]==0xff )
                        {
                            judge_if_alive=0;
                            break;
                        }
                        else if(game->obstacle[0]->object_status==Pter_2 &&pter2[y*20*2+x]==0xff )
                        {
                            judge_if_alive=0;
                            break;
                        }
                    }
                        
            }
    }
    
//	LCD_Show_specialPic(0,0,51,63,image);
	for(int i=0 ;i< 52*2 ;i++)
		free(image[i]);
	
    free(image);
}


int main(void)
{
    
    IO_init(); // init OLED
    Game_Information *Game = Game_init();
    

    timer = (timer + 10) % U16_MAX;
    // every zhen
    startmenu();
    obstacle_last_time = 0;
    obstacle_interval = 16;
    int last_state = Game->state;
    while (TRUE)
    {
        // update input
        ProcessInput();
        // logic define
        if (Game->state == Home_page) // last status is Home_page
        {
            if (last_state != Game->state)
            {
                LCD_Clear(BLACK);
                startmenu();
            }
            last_state = Game->state;
            if (getButton0 || getButton1)
            {
                int choice = Home_choice();
                if (!choice)
                    Game->state = Play_page; // uodate status
                else
                    Game->state = Settings_page; // uodate status
                delay_1ms(10);
                timer = (timer + 10) % U16_MAX;
                if (!choice)
                {
                    LCD_Clear(BLACK);
                    Draw();
                    delay_1ms(10);
                }
                   
            }
        }
        else if (Game->state == Play_page) // last status is Play_page
        {
        	
            // update four part: 1 trex,2 obstracle, 3 ground ,4 score
           // LCD_Clear(BLACK);
            last_state = Game->state;
           // bool *target_img = (image_ptr == image0) ? image1 : image0; 
            update_ground(Game);
            update_obstracle(Game);
            // update trex
            if (getButton0 == 1 && ( Game->Trex->trex_status == Walk_1 || Game->Trex->trex_status == Walk_2 ))// Jump start
            {
                Game->Trex->trex_status=Jump;
                Game->Trex->if_rise=1;
                Game->Trex->jump_height=1;
                LCD_ShowPic(25,44-Game->Trex->jump_height,44,63-Game->Trex->jump_height,trex3,sizeof(trex3)/sizeof(unsigned char));
            }
            else if (getButton1 == 1 && Game->Trex->trex_status != Jump) // Squat
            {
                if(Game->Trex->trex_status== Walk_1 ||Game->Trex->trex_status== Walk_2|| Game->Trex->trex_status== Squat_2)
                {
                    Game->Trex->trex_status=Squat_1;
                    LCD_ShowPic(25,44,51,63,trex4,sizeof(trex4)/sizeof(unsigned char));
                }
                else if(Game->Trex->trex_status== Squat_1)
                {
                    Game->Trex->trex_status=Squat_2;
                    LCD_ShowPic(25,44,51,63,trex5,sizeof(trex5)/sizeof(unsigned char));
                }
               
            }
            else // walk or keep jump status
            {
                if(Game->Trex->trex_status== Walk_1 ||Game->Trex->trex_status== Walk_2  )
                {
                    if(Game->Trex->trex_status== Walk_1)
                    {
						LCD_ShowPic(25,44,44,63,trex2,sizeof(trex2)/sizeof(unsigned char));
						Game->Trex->trex_status= Walk_2;
                    }
                    else 
                    {
                    	LCD_ShowPic(25,44,44,63,trex1,sizeof(trex1)/sizeof(unsigned char));
						Game->Trex->trex_status= Walk_1;
                    }
                }
                else if(Game->Trex->trex_status== Jump)
                {
                    if(Game->Trex->if_rise==1)
                    {
                        if(Game->Trex->jump_height == JUMP_MAX_HEIGHT)// magin
                        {
                            Game->Trex->if_rise=-1;
                            LCD_DeletePic(25,44-Game->Trex->jump_height,51,63-Game->Trex->jump_height);
                            Game->Trex->jump_height--;
                            LCD_ShowPic(25,44-Game->Trex->jump_height,44,63-Game->Trex->jump_height,trex3,sizeof(trex3)/sizeof(unsigned char));
                            //LCD (trex3)
                        }
                        LCD_DeletePic(25,44-Game->Trex->jump_height,51,63-Game->Trex->jump_height);
                        Game->Trex->jump_height++;
                        LCD_ShowPic(25,44-Game->Trex->jump_height,44,63-Game->Trex->jump_height,trex3,sizeof(trex3)/sizeof(unsigned char));
                        //LCD(trex3)
                    }
                    else // down
                    {
                        if(Game->Trex->jump_height == 1)
                        {
                            Game->Trex->if_rise=0;
                            Game->Trex->jump_height--;
                            Game->Trex->trex_status=Walk_1;
                            LCD_ShowPic(25,44,44,63,trex1,sizeof(trex1)/sizeof(unsigned char));
                            //LCD(trex1);
                        }
                        LCD_DeletePic(25,44-Game->Trex->jump_height,51,63-Game->Trex->jump_height);
                        Game->Trex->jump_height--;
                        LCD_ShowPic(25,44-Game->Trex->jump_height,44,63-Game->Trex->jump_height,trex3,sizeof(trex3)/sizeof(unsigned char));
                        //LCD(trex3)
                    }
                }
                else if(Game->Trex->trex_status==Squat_1 || Game->Trex->trex_status==Squat_2)
                {
                	if(Game->Trex->trex_status== Squat_1)
                    {
                    	LCD_DeletePic(25,44,51,63);
						LCD_ShowPic(25,44,44,63,trex2,sizeof(trex2)/sizeof(unsigned char));
						Game->Trex->trex_status= Walk_2;
                    }
                    else 
                    {
                    	LCD_DeletePic(25,44,51,63);
                    	LCD_ShowPic(25,44,44,63,trex1,sizeof(trex1)/sizeof(unsigned char));
						Game->Trex->trex_status= Walk_1;
                    }
                }
            }
            
            judge_trex_alive(Game);
            if (!judge_if_alive)
            {
                Game->state = End_page; // update status
                gameover();
            }
                
            else
                game_run_time++;
            // draw
            delay_1ms(10);
            //Draw(target_img);
          	//image_ptr = (image_ptr == image0) ? image1 : image0; // switch buffer
            show_score(game_run_time);//this is a fucntion in assembly
            if (!judge_if_alive)
            	delay_1ms(600);

        }
        else if (Game->state == Settings_page) // last status is Settings_page, change diff to control the difficulty
        {

            if (last_state != Game->state)
            {
                LCD_Clear(BLACK);
                settingsmenu();
            }
            last_state = Game->state;
            if (getButton0 || getButton1)
            {
                int choice=make_choice();
                delay_1ms(10);
                timer = (timer + 1000) % U16_MAX;
                if(choice)
                    diff=22;
                else diff=35;
                Game->state = Home_page; // uodate status
            }
        }
        else if (Game->state == End_page) // last status is End_page
        {
            // write end game string to the middle of screen
            if (last_state != Game->state)
            {
                LCD_Clear(BLACK);
                endmenu(game_run_time,bit_num(game_run_time));
            }
            last_state = Game->state;
            if (getButton0 || getButton1)
            {
                int choice=make_choice();
                delay_1ms(10);
                timer = (timer + 10) % U16_MAX;
                if(!choice)//choose to retry
                {
                    Game = reinit_game(Game);// uodate status
                    obstacle_last_time = 0;
    				obstacle_interval = 16;
                    game_run_time=0;
                    timer = (timer + 10000) % U16_MAX;
                    count=0;
                    judge_if_alive=1;
                    diff=35;
                   
                }
                else //quit game
                    break;
            }
            
        }

        // This delay can be treat as difficulty of the game
        // Smaller delay make snake move faster, thus more difficult
        if (Game->state == Play_page)
        {
           
            delay_1ms(diff);
            timer = (timer + diff*10) % U16_MAX;
                
            
        }
    }
    // Game Over
    LCD_Clear(BLACK);
    delete_game(Game);
    
    
    


    return 0;
}