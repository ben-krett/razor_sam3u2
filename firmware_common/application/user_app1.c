/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
static u32 * UserApp1_u32p_combo;
static u8 UserApp1_u8_comboLength;
static u32 UserApp1_u32_LedList [] = {WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};

//static u32 UserApp1_u32Timeout;  /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

void acknowledgeAll (void)
{
        ButtonAcknowledge(BUTTON0);
        ButtonAcknowledge(BUTTON1);
        ButtonAcknowledge(BUTTON2);
        ButtonAcknowledge(BUTTON3);

}

u32 check_pressed (void) {
  if (WasButtonPressed(BUTTON0)) 
  {
      ButtonAcknowledge(BUTTON0);
      return BUTTON0;
    }
    if (WasButtonPressed(BUTTON1)) 
    {
      ButtonAcknowledge(BUTTON1);
      return BUTTON1;
    }
    if (WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      return BUTTON2;
    }
   if (WasButtonPressed(BUTTON3)) 
   {
      ButtonAcknowledge(BUTTON3);
      return BUTTON3;
    }
  return NOBUTTON;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  LedOff(RED);
  LedOff(BLUE);
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(CYAN);
  LcdCommand(LCD_CLEAR_CMD);
  LcdMessage(LINE1_START_ADDR, "Starting up...");


  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_StartUpTimer;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */

static void UserApp1SM_StartUpTimer(void)
{
  static u32 u32Counter = 0;
  u32Counter++;
  if (WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    UserApp1_u32p_combo = calloc(sizeof(u32), 10);
    UserApp1_u8_comboLength = 0;
    LcdCommand(LCD_CLEAR_CMD);
    LcdMessage(LINE1_START_ADDR, "Set a password:");
    acknowledgeAll();
    UserApp1_pfStateMachine = UserApp1SM_SetPassword;
  }
  else if (u32Counter == 3000)
  {
    UserApp1_u32p_combo = calloc(sizeof(u32), 10);
    u32 temp [10] = {BUTTON0, BUTTON1, BUTTON2};
    UserApp1_u8_comboLength = 3;
    for (int i = 0; i < 10; i++)
    {
      UserApp1_u32p_combo[i] = temp[i];
    }
    LcdCommand(LCD_CLEAR_CMD);
    LcdMessage(LINE1_START_ADDR, "Enter the password:");
    acknowledgeAll();
    UserApp1_pfStateMachine = UserApp1SM_Locked;
  }
}

static void UserApp1SM_SetPassword(void)
{
  static u32 u32Counter = 0;
  u32Counter++;
  if (u32Counter%1000 < 500) 
  {
    LedOn(RED);
    LedOff(GREEN);
  }
  else
  {
    LedOn(GREEN);
    LedOff(RED);
  }
  u32 u32Button = check_pressed();
  if ((u32Button == BUTTON0 || u32Button == BUTTON1 || u32Button == BUTTON2) && UserApp1_u8_comboLength < 10)
  {
    UserApp1_u32p_combo[UserApp1_u8_comboLength] = u32Button;
    char charButton = u32Button + '1';
    LcdMessage(LINE2_START_ADDR + UserApp1_u8_comboLength, &charButton);
    LcdClearChars(LINE2_START_ADDR + UserApp1_u8_comboLength + 1, 5);
    UserApp1_u8_comboLength++;

  }
  else if (u32Button == BUTTON3 && UserApp1_u8_comboLength > 0)
  {
    LcdCommand(LCD_CLEAR_CMD);
    LcdMessage(LINE1_START_ADDR, "Enter the password:");
    UserApp1_pfStateMachine = UserApp1SM_Locked;
    LedOff(GREEN);
    LedOff(RED); 
  }
  
}

static void UserApp1SM_Locked(void)
{
  static bool bool_errorMade = FALSE;
  static int int_userStep = 0;
  u32 u32Button = check_pressed();  
  if (u32Button == BUTTON0 || u32Button == BUTTON1 || u32Button == BUTTON2)
  {
    if (int_userStep >= UserApp1_u8_comboLength)
    {
      bool_errorMade = TRUE;
    } 
    else if (u32Button != UserApp1_u32p_combo[int_userStep])
    {
      bool_errorMade = TRUE;
    }
    LcdMessage(LINE2_START_ADDR + int_userStep, "*");
    int_userStep++;
  }
  else if (u32Button == BUTTON3)
  {
    if (int_userStep < UserApp1_u8_comboLength)
    {
      bool_errorMade = TRUE;
    }
    
    if (bool_errorMade)
    {
      LcdCommand(LCD_CLEAR_CMD);
      LcdMessage(LINE1_START_ADDR, "Wrong Code!");
      UserApp1_pfStateMachine = UserApp1SM_WrongCode;
      bool_errorMade = FALSE;
      int_userStep = 0;
    } 
    else
    {
      LcdCommand(LCD_CLEAR_CMD);
      LcdMessage(LINE1_START_ADDR, "Correct Password!");
      LcdMessage(LINE2_START_ADDR, "Press B4 to reset");
      UserApp1_pfStateMachine = UserApp1SM_Unlocked;
      bool_errorMade = FALSE;
      int_userStep = 0;
    }
  } 
}

static void UserApp1SM_WrongCode(void)
{
  static u32 u32Counter = 0;
  u32Counter++;
  if (u32Counter % 1000 < 500)
  {
    LedOn(RED);
    LedOff(LCD_GREEN);
    LedOff(LCD_BLUE);
  }
  else
  {
    LedOff(RED);
    LedOn(LCD_GREEN);
    LedOn(LCD_BLUE);
  }
  
  u32 u32Button = check_pressed();
  
  if (u32Counter > 4000 && u32Button != NOBUTTON)
  {
    LedOff(RED);
    LedOn(LCD_GREEN);
    LedOn(LCD_BLUE);
    u32Counter = 0;
    LcdCommand(LCD_CLEAR_CMD);
    LcdMessage(LINE1_START_ADDR, "Enter the password:");
    UserApp1_pfStateMachine = UserApp1SM_Locked; 
  }
}
static void UserApp1SM_Unlocked(void)
{
  static u32 timer = 0;
  timer++;
  for (int i = 0; i < 8; i++) 
  {
    if (((timer%200) < (i+1)*25) && ((timer%200) > (i*25))) 
    {
      LedOn(UserApp1_u32_LedList[i]);
    } 
    else 
    {
      LedOff(UserApp1_u32_LedList[i]);
    }
  }
  u32 u32Button = check_pressed();
  if (u32Button != NOBUTTON && u32Button != BUTTON3)
  {
    timer = 0;
    LcdCommand(LCD_CLEAR_CMD);
    LcdMessage(LINE1_START_ADDR, "Enter the password:");
    UserApp1_pfStateMachine = UserApp1SM_Locked;
    for (int i = 0; i < 8; i++) 
    {
      LedOff(UserApp1_u32_LedList[i]);
    }
  }
  else if (u32Button == BUTTON3)
  {
    for (int i = 0; i < 8; i++) 
    {
      LedOff(UserApp1_u32_LedList[i]);
    }
    timer = 0;
    free(UserApp1_u32p_combo);
    UserApp1_u32p_combo = calloc(sizeof(u32), 10);
    UserApp1_u8_comboLength = 0;
    LcdCommand(LCD_CLEAR_CMD);
    LcdMessage(LINE1_START_ADDR, "Set a password:");

    UserApp1_pfStateMachine = UserApp1SM_SetPassword;
  }
}
      
      
static void UserApp1SM_Idle(void)
{
  
  /*
  static u32 timer = 0;
  static bool ERROR_MADE = FALSE; // check if an error has been made yet
  static bool bool_IS_ERROR_SUBMITTED = FALSE; // are we in error mode?, is red light flashing
  static bool bool_LOCK_COMPLETED = FALSE;
  static u32 u32_USER_STEP = 0; // how many buttons are in the user's current input
  int BUTTON_PRESSED = check_pressed();
  timer++;
 
  if (bool_IS_ERROR_SUBMITTED) 
  {
    if (BUTTON_PRESSED != -1)
    {
      bool_IS_ERROR_SUBMITTED = FALSE;
      LedOff(RED);
    }
    else
    {
      if (timer%200 < 100) 
        LedOn(RED);
      else 
        LedOff(RED);
    }
    //led code
  }
  else if ((BUTTON_PRESSED == 0 || BUTTON_PRESSED == 1 || BUTTON_PRESSED == 2) && !bool_LOCK_COMPLETED) //input button pressed
  {
    if (u32_USER_STEP >= u8_COMBO_LEN) //is wrong
    {
      ERROR_MADE = TRUE;
    }
    else if (BUTTON_PRESSED != BUTTON_COMBO[u32_USER_STEP])
    {
      ERROR_MADE = TRUE;
    }
    u32_USER_STEP++;
  } 
  else if (BUTTON_PRESSED == 3) 
  {
    if (u32_USER_STEP ==  u8_COMBO_LEN && !ERROR_MADE) 
    {
      bool_LOCK_COMPLETED = TRUE;
    } 
    else 
    {
      ERROR_MADE = FALSE;
      bool_IS_ERROR_SUBMITTED = TRUE;
      u32_USER_STEP = 0;
      LedOn(RED);
    } 
  }
  else if (bool_LOCK_COMPLETED)
  {
    if (BUTTON_PRESSED != -1)
    {
      bool_LOCK_COMPLETED = FALSE;
      u32_USER_STEP = 0;
      for (int i = 0; i < 8; i++) 
      {
        LedOff(LED_LIST[i]);
      }
    } 
    else
    {
      for (int i = 0; i < 8; i++) 
      {
        if (((timer%200) < (i+1)*25) && ((timer%200) > (i*25))) 
        {
          LedOn(LED_LIST[i]);
        } 
        else 
        {
          LedOff(LED_LIST[i]);
        }
      }
    }
  }
  
  */
  
    
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
