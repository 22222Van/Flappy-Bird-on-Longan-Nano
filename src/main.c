#include "lcd/lcd.h"
#include "utils.h"
#include "assembly/example.h"
#include <stdlib.h>


void Inp_init(void)
{
  rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_GPIOC);

  gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ,
            GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
  gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ,
            GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}

void IO_init(void)
{
  Inp_init(); // inport init
  Lcd_Init(); // LCD init
}

/* y is in [77,148]*/
void draw_wall(u16 x, u16 y, u16 color)
{

  LCD_DrawRectangle(x, y, x + 2, 149, color);
  LCD_DrawRectangle(x, 41, x + 2, y - 35, color);
}

void Board_self_test(void)
{
  
  // Get_Button(BUTTON_1);
  // Get_Button(BUTTON_2);
  // while (1)
  // {
  //   result = draw(any_button, (u8 *)"Press",(u8 *)"any",(u8 *)"button",(u8 *)"to",(u8 *)"continue");

  //   if (Get_Button(BUTTON_1) || Get_Button(BUTTON_2))
  //   {
  //     any_button = 1;
  //   }
  //   if(result==1){
  //     break;
  //   }
  //   delay_1ms(200);
  // }
  draw((u8 *)"  Press    any     button      to    continue  ");

  LCD_Clear(BLACK);
  uint32_t delay_time = 50;
  uint64_t start_mtime, delta_mtime, tmp;
  start_mtime = get_timer_value();

  uint32_t count = 0;  // joystick cooldown
  u16 select_diff = 0; // 0 for ez,1 for hard
  while (1)
  {
    LCD_ShowString(0, 0, (u8 *)"Select", BLUE);
    LCD_ShowString(0, 25, (u8 *)"Mode", BLUE);

    /* code */
    if (select_diff == 0)
    {
      LCD_DrawRectangle(15, 85, 55, 115, BLACK);
      LCD_DrawRectangle(15, 55, 55, 85, RED);
      LCD_ShowString(20, 60, (u8 *)"EAZY", RED);
      LCD_ShowString(20, 90, (u8 *)"HARD", WHITE);
    }
    else
    {
      LCD_DrawRectangle(15, 55, 55, 85, BLACK);
      LCD_DrawRectangle(15, 85, 55, 115, RED);
      LCD_ShowString(20, 60, (u8 *)"EAZY", WHITE);
      LCD_ShowString(20, 90, (u8 *)"HARD", RED);
    }
    if ((Get_Button(JOY_LEFT) || Get_Button(JOY_RIGHT)) && (count == 0))
    {
      count = 300;
      select_diff = 1 - select_diff;
    }

    // draw();
    do
    {
      delta_mtime = get_timer_value() - start_mtime;
    } while (delta_mtime < (SystemCoreClock / 4000.0 * delay_time));
    tmp = get_timer_value();
    do
    {
      start_mtime = get_timer_value();
    } while (start_mtime == tmp);
    if (count >= 20)
    {
      count -= 50;
    }
    // LCD_ShowString(30, 25, (u8 *)"EAZY", BLACK);
    // LCD_ShowString(90, 25, (u8 *)"HARD", BLACK);
    // LCD_DrawRectangle(85, 20, 125, 50, BLACK);
    // LCD_DrawRectangle(25, 20, 65, 50, BLACK);
    // LCD_ShowString(0, 0, (u8 *)"Select Difficultiy", BLACK);

    // draw();
    if (Get_Button(JOY_CTR))
    {
      break;
    }
  }
  LCD_Clear(BLACK);

  u16 fall_speed = 0; // Hard mode is less controlable
  u16 up_speed = 4 - 2 * select_diff;

  u16 damage = 0;   // invincible trigger
  u16 inv_time = 0; // invincible time count

  u16 life = 3;
  u16 score = 0;

  // uint64_t start_time = get_timer_value();

  u16 bird_height = 95;

  // delay_time = 50;

  u16 tail[10] = {0}; // Trail_y

  u16 wall[78] = {0}; // Wall_y
  u16 wall_cd = (1 - select_diff) * 30 + 40;
  u16 wall_counter = wall_cd;

  u16 suc = 0; // For dynamic wall hit

  while (1)
  {

    LCD_DrawLine(0, 40, LCD_W, 40, YELLOW);   // Upper static wall
    LCD_DrawLine(0, 150, LCD_W, 150, YELLOW); // Lower static wall

    // Draw dynamic wall
    for (u16 k = 0; k < 78; k++)
    {
      if ((wall[k] >= 77) && (wall[k] <= 148))
      {
        draw_wall(k, wall[k], GRED);
      }
    }

    // Draw tail
    for (u16 j = 0; j < 9; j++)
    {
      if ((tail[j] > 41) && (tail[j] < 149) && (tail[j + 1] > 41) && (tail[j + 1] < 149))
      {
        LCD_DrawLine(j, tail[j], j + 1, tail[j + 1], GBLUE);
        // LCD_DrawPoint(j, tail[j], BLUE);
      }
    }

    // Settle damage
    if ((damage > 0))
    {
      if ((inv_time == 0))
      {
        bird_height = 95;
        fall_speed = 0;
        inv_time = 30 * delay_time;
        if (life > 0)
        {
          life--;
        }
        else
        {
          life = life + 114;
        }
      }
      damage = 0;
    }

    // Deal invincible
    if (inv_time > 0)
    {
      inv_time -= delay_time;
      LCD_DrawCircle(10, bird_height, 2, RED);
    }
    else
    {
      LCD_DrawCircle(10, bird_height, 2, WHITE);
    }

    LCD_ShowString(0, 0, (u8 *)"Score:", BLUE);
    LCD_ShowNum(45, 0, score, 3, BLUE);

    LCD_ShowString(0, 20, (u8 *)"Life:", BLUE);
    LCD_ShowNum(40, 20, life, 3, BLUE);
    // draw();

    // Delay
    do
    {
      delta_mtime = get_timer_value() - start_mtime;
    } while (delta_mtime < (SystemCoreClock / 4000.0 * delay_time));
    tmp = get_timer_value();
    do
    {
      start_mtime = get_timer_value();
    } while (start_mtime == tmp);

    // Erase dynamic wall
    for (u16 k = 0; k < 78; k++)
    {
      if ((wall[k] >= 77) && (wall[k] <= 148))
      {
        draw_wall(k, wall[k], BLACK);
      }
    }

    // Shift walls
    for (u16 k = 0; k < 77; k++)
    {
      wall[k] = wall[k + 1];
    }
    // New wall
    if (wall_counter == 0)
    {
      wall_counter += wall_cd;
      wall[77] = rand() % 72 + 77;
    }
    else
    {
      wall[77] = 0;
      wall_counter--;
    }

    // Erase tail
    for (u16 j = 0; j < 9; j++)
    {
      if ((tail[j] > 41) && (tail[j] < 149) && (tail[j + 1] > 41) && (tail[j + 1] < 149))
      {
        LCD_DrawLine(j, tail[j], j + 1, tail[j + 1], BLACK); // Lower static wall
      }
    }

    // Shift samples
    for (u16 j = 0; j < 9; j++)
    {
      tail[j] = tail[j + 1];
    }
    // New sample
    tail[9] = bird_height;

    LCD_DrawCircle(10, bird_height, 2, BLACK);
    if (Get_Button(JOY_LEFT))
    {
      if (bird_height > 40)
      {
        fall_speed = 0;
        bird_height -= up_speed;
      }
    }
    else
    {
      if (bird_height < 150)
      {
        fall_speed += (select_diff + 1);
        bird_height += fall_speed;
      }
      if (bird_height > 150)
      {
        bird_height = 150;
      }
    }

    // Hit static wall
    if (((bird_height >= 149) || (bird_height <= 41)) && (inv_time == 0))
    {
      damage++;
    }

    // Dynamic wall policy
    // Hit detect
    if (((wall[8]) > 0) || ((wall[9]) > 0) || ((wall[10]) > 0) || ((wall[11]) > 0) || ((wall[12]) > 0))
    {
      for (u16 m = 8; m <= 12; m++)
      {
        if (wall[m] > 0)
        {
          suc = wall[m];
          break;
        }
      }
      // radius is 2
      // gap is 35
      if (!((bird_height < (suc - 2)) && (bird_height > (suc - 33))))
      {
        damage++;
      }
    }
    // Score detect
    if (((wall[8]) > 0) && (damage == 0))
    {
      score++;
    }
  }
}


// Previous code in lab12

// while (1)
// {
//   if (Get_Button(JOY_LEFT))
//   {
//     LCD_ShowString(5, 25, (u8 *)"L", BLUE);

//   }
//   if (Get_Button(JOY_DOWN))
//   {

//     // LCD_Clear(BLACK);//For lab12

//     LCD_ShowString(25, 45, (u8 *)"D", BLUE);
//     // LCD_ShowString(60, 25, (u8*)"TEST", GREEN);
//     LCD_ShowString(60, 25, (u8 *)"TEST", RED);
//   }
//   else
//   {
//     LCD_ShowString(60, 25, (u8 *)"TEST", WHITE);
//   }
//   if (Get_Button(JOY_UP))
//   {
//     LCD_ShowString(25, 5, (u8 *)"U", BLUE);
//   }
//   if (Get_Button(JOY_RIGHT))
//   {
//     LCD_ShowString(45, 25, (u8 *)"R", BLUE);
//   }
//   if (Get_Button(JOY_CTR))
//   {
//     LCD_ShowString(25, 25, (u8 *)"C", BLUE);
//   }
//   if (Get_Button(BUTTON_1))
//   {
//     // LCD_ShowString(60, 5, (u8*)"SW1", BLUE);

//     LCD_ShowString(60, 45, (u8 *)"SW2", BLUE);
//   }
//   if (Get_Button(BUTTON_2))
//   {
//     // LCD_ShowString(60, 45, (u8*)"SW2", BLUE);

//     LCD_ShowString(60, 5, (u8 *)"SW1", BLUE);
//   }
//   draw();
// delay_1ms(10);
//   LCD_Clear(BLACK);
// }

int main(void)
{
  IO_init();
  Board_self_test();
}
