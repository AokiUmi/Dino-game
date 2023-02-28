# 关于play函数部分声明

## 初始化常量

```c
// some basic constants
#define JUMP_MAX_HEIGHT
#define CACTUS_MAX_HEIGHT
#define GROUND_HEIGHT
#define PTER_HEIGHT
#define TREX_WLAK_HEIGHT
#define TREX_SQUAT_HEIGHT
#define OBSTACLE_START_POS
#define INITIAL_GROUND1_POS 0
#define INITIAL_GROUND2_POS 160
```

列举了很多和后面程序相关的常量，需要运行他给的`converter.c`这个函数自己找一下每个图片长宽之类的数据，做好记录，还有很多常量可以自己添加，有些是后面操作需要自己定义的

## 主函数

```c
        else if (Game->state == Play_page) // last status is Play_page
        {
            // update four part: 1 trex,2 obstracle, 3 ground ,4 score
            last_state = Game->state;
            update_ground(Game, target_img);
            update_obstracle(Game, target_img);
            // update trex
            if (getButton0 == 1) // Jump start
            {
            }
            else if (getButton1 == 1) // Squat
            {
            }
            else // walk or keep jump status
            {
            }
            if (!judge_if_alive)
            {
                Game->state = End_page; // update status
                game_over();
            }

            else
                game_run_time++;
            // draw
            Draw(target_img);
            show_score(game_run_time);//this is a fucntion in assembly

        }
```

主要思路是

1. 在主循环外先初始化最初的游戏图像，包括 恐龙，地板两个小图 分数0不显示，先把这个图像存到`image_ptr `
   
   ```c
   void init_image(uint8_t *image) // play start time=0
   {
   }
   ```

2. 当`Game->state == Play_page`正式进入开始游戏的第一帧，此时我们在上一帧已经显示了`image0`所以每次当这个时候，我们一定是在上一帧的`image_ptr`的基础上进行update修改，这个修改存在不同于`image_ptr`的另一个数组`target_img`作为当前帧的图像

3. 以地板，障碍物，恐龙的顺序以此将小的图片写入主屏幕图像数组，分数不是图像，用的是`LCD_ShowNum()`这个函数，所以在`LCD_Showpic()`之后单独加上去就行

## 具体实现

~~**将小图写入大屏幕的转换函数**~~

```c
void WriteToImageBuffer(uint8_t *image, uint8_t * object,int width,int height) // write each little object to the whole screen
{
    return;
}
```

需要将两个数组展开，注意image有固定的长宽，小图片长宽是常量自己输入，展开为二维矩阵再对应输入，需要自己计算一下公式

每个小图的数组在`"img.h"`*

### 地板

地板有两个小图`g1.jpg,g2,jpg`交替出现组成整个地板

所以有两个int指向每张小图其实出现的点的横坐标位置，纵坐标由于图片宽度都是固定的

```c
struct Game_Information
{
    Trex_Information *Trex;
    Game_Status state;
    int obstacle_number;
    Obstacle_Information **obstacle;
    int ground1_pos; // two ground pic using it both
    int ground2_pos;
};
```

写的时候出屏幕的部分要跳过只保留屏幕中显示的部分，需要自己处理一下，这两个pos随时间不断++循环一次后回到初始设定值重新循环

```c
    new_game->ground1_pos = INITIAL_GROUND1_POS;
    new_game->ground2_pos = INITIAL_GROUND2_POS;
```

一开始一个地板应该是出框的可以研究一下图片宽度是不是和屏幕宽度完全一致做点调整，屏幕宽度应该是160

### 障碍物

这个比较复杂，需要随机一个`obstacle_interval`要保证一个最小值和最大值来随机，这个表示的是每个障碍物之间的距离

game开始的时候之前先把这两个初始化`obstacle_last_time=0;obstacle_interval=16;`

当`game_run_time-obstacle_last_time==obstacle_interval`就放障碍物，所以障碍物update函数不仅更新之前障碍物的位置，也要在适当的时候加减障碍物，用` Generate_U16(lower,upper,timer)`更新interval,由于不能用`rand()`,需要自己弄随机种子

```c
int obstacle_number;
Obstacle_Information **obstacle;
```

这个根据屏幕上有多少障碍物动态实现，**这个指针数组应该动态变化**

```c
struct Obstacle_Information
{
    int x;
    Obstacle object_status;
};
enum Obstacle
{
    cactus_1=0,cactus_2, pter_1, pter_2; // obstacle status
};
```

专门写的结构体存每个障碍物种类和出现的横坐标，因为纵向宽度同样也是固定的

注意鸟有两个图，分别交替出现所以每次更新的时候都根据上一帧的状态更换图片，每一帧往坐平移一格

仙人掌只是两种不同的图不用更新状态，这两个仙人掌随机刷

### 恐龙

首先我们设定蹲是需要长按按钮的，所以只有`getButton1==1`才执行蹲

而跳只需要按一下，所以在没有按钮输入的时候有两种情况 跳跃途中和走

```c
enum Trex_Status
{
    Walk_1 = 0, Walk_2, Jump, squat_1, squat_2; // trex status
};
```

这是恐龙的5个状态

```c
struct Trex_Information
{
    Trex_Status trex_status; // represent trew's status
    int jump_height;         // if trex_status=Jump, memorize now height
    int if_rise;             //  if_rise==1 ,trex_status=Jump and it is rise now or it is down
};
```

#### 走和蹲

这个状态比较简单，有两个图片交替进行，更新并判断状态就行

因为不存在横纵坐标的变化只要把图片写进去就行，恐龙的合纵初始坐标是常数请自己尝试后根据地板的位置定一下

#### 跳跃

跳的时候比较麻烦是要判定现在实在跳还是跳约上升过程还是跳跃下降过程

起跳的时候`(getButton0 == 1)` 直接为更新下一帧起跳高度为1的图，再把`if_rise=1,jump_height=1`

之后用这两个参数来更新后续跳跃过程，其中当`jump_height==JUMP_MAX_HEIGHT`立马更新状态，直到高度为0，将状态变回`walk_1`

最后更新完这些东西再把恐龙输入图片，横坐标固定，纵坐标不固定。

**此时在写入恐龙图片数组的时候我们已经将障碍物移动更新过了并且写入了数组，我们要边写恐龙的部分边判断有没有像素点重叠，即有没有死亡**

若有重叠直接更新`judge_if_alive=0`结束游戏，但图片输入不打断

至此流程全部结束

## 如何调试

自己将图片展开为二位数组打开打印出来很简单，`0x00=' ',0xff='.'`即可

电脑输入两个button的状态输出每一帧的图片就行