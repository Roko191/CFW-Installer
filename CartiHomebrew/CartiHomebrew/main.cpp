/*
    WARNING!
        This Code = REAL BAD!


        CFW installer -> Troll app
        Coded by Roko191

        License: Idk yet to see (Apache one probably!)

        About: Point of this app is to troll but also to
        teach a lesson to not trust every piece of shit on
        the internet. It uses Sony troll vaule of sceKernelIccBuzzer()
        to do infinitive beeping and 

        Thanks to:
            OpenOrbis team -> toolchain, libaries, help,...
            kiwidog -> helping me with setting up and everything
            hoppers -> putting your PS4 trought pain of testing this shit
            0x199 -> help with this homebrew, web development and being like a mentor to me trought my learing path
            bucanero -> Custom dialog fuctions which make MsgDialog.h really easy to use
        
        OpenOrbis Toolchain by OpenOrbis Team ==> https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain
        PS4 Skeleton by AlAzif ==> https://github.com/Al-Azif/ps4-skeleton

        Assets:
            Font: 
                -> Roboto by Google { Best font ever } ==> https://fonts.google.com/specimen/Roboto

        Note: Please ignore dir name CartiHomebrew thanks
*/

// General C/C++ libaries
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <inttypes.h>

#include <sstream>

// Orbis Libaries
#include <orbis/libkernel.h>
#include <orbis/UserService.h>
#include <orbis/SystemService.h>
#include <orbis/CommonDialog.h>
#include <orbis/MsgDialog.h>
#include <orbis/Sysmodule.h>

// Custom Libaries by OrbisTeam
#include "../_common/graphics.h"
#include "../_common/log.h"
#include "../_common/png.h"
#include "../_common/controller.h"

// Custom Libaries by AlAzif
#include "../include/utils.h"

#include "../external/libLog.h"
#include "../external/notifi.h"

// Custom MsgDialog libary from bucanero
#include "dialog.h"

// My custom Libaries
#include "color.h"

// Defining stuff and global variables

// Dimensions of screen we are working with
// Scene Element takes this stuff as argument
#define FRAME_WIDTH     1920
#define FRAME_HEIGHT    1080
#define FRAME_DEPTH        4

// Font information
// Aruguments for font size (Diffrent number diffrent size)
#define FONT_SIZE_LARGE  90
#define FONT_SIZE_SMALL   40

// Logging
std::stringstream debugLogStream;

//Color variables
Color bgColor;
Color fgColor;

// Font faces
FT_Face fontLarge;
FT_Face fontSmall;

int frameID = 0;

// This function will display p_Message, then "cleanly" close the application
void fail(const char *p_Message) {
  logKernel(LL_Fatal, "%s", p_Message);
  notifi(NULL, "%s", p_Message);
  sceSystemServiceLoadExec("exit", NULL);
}

int main()
{
    // Local variables
    // Mostly used for intilazing and maintaing stuff 
    int rc;
    int video;
    int curFrame = 0;

    // No buffering
    // You will see this in every homebrew (You dont need to worry about it)
    setvbuf(stdout, NULL, _IONBF, 0);

    // This is just testing of entering the sandbox mode
    // Jailbreak
    jailbreak();

    // Load MsgDialog
    if (sceSysmoduleLoadModule(ORBIS_SYSMODULE_MESSAGE_DIALOG) < 0)
    {
        printf("Failed to load dialog!");
        return -1;
    }
    //Intilaze common dialog
    if (sceCommonDialogInitialize() < 0)
    {
        printf("Failed to init CommonDialog!");
        return -1;
    }

    notifi(NULL, "%s", "CFW Installer... \n Coded by Roko191");

    // Showing warning dialog with OK button
    show_dialog(0, "WARNING! I dont take responsibility for any kind of damage to your console or you!");

    //We go back to jail because app crash if we dont because we are in shell core process duh...
    unjailbreak();

    /* Thats it for entering the sandbox */

    //Intilazie Audio, Video, Scene, Controller, load modules, sandobox, ...
    DEBUGLOG << "Starting program (Intilazing stuff...)";

    // Create a 2D scene
    DEBUGLOG << "Creating a 2D scene...";

    // This should make the new scene where we can draw text, images, shapes and whole lotta more... 
    auto scene = new Scene2D(FRAME_WIDTH, FRAME_HEIGHT, FRAME_DEPTH); 
    
    if(!scene->Init(0xC000000, 2))
    {
        // In this case scene creating proccess fails
    	DEBUGLOG << "Failed to initialize 2D scene";
    }
    /* THATS IT FOR SCENE*/

    // Set colors
    // These are just basic colors that are going to be used
    // Difrent colors can be used in fuctions in any given time
    bgColor = black();
    fgColor = white();


    // Initialize the font faces with arial (must be included in the package root!)
    // Pointers pointing to font files
    // Bold is used for large and Regular for small font
    const char *roboto = "/app0/assets/fonts/Roboto-Regular.ttf";
    const char *robotoBold = "/app0/assets/fonts/Roboto-Bold.ttf";
    
    DEBUGLOG << "Initializing font (" << roboto << ") and (" << robotoBold << ")";

    if(!scene->InitFont(&fontLarge, robotoBold, FONT_SIZE_LARGE))
    {
        // In this case init of Roboto Bold failed
    	DEBUGLOG << "Failed to initialize large font '" << robotoBold << "'";
    	for(;;);
    }

    if(!scene->InitFont(&fontSmall, roboto, FONT_SIZE_SMALL))
    {
        // In this case init of Roboto Regular failed
    	DEBUGLOG << "Failed to initialize small font '" << roboto << "'";
    	for(;;);
    }
    /* Thats it for fonts */

    auto controller = new Controller();
    
    if(!controller->Init(-1))
    {
    	DEBUGLOG << "Failed to initialize controller";
    	for(;;);
    }

    // Counter variable
    int i = 0;

    // Storing fw info inside FW variable    
    uint64_t FW = get_firmware();

     // Setting up FW variable
    char FWchar[256];

    // Savinng content of FW into FWchar
    // We do this beacuse notifi needs char *value as argument
    snprintf(FWchar, sizeof(FWchar), "%" PRIu64, FW);

    // This check is here because now it just stores 9 and I want it 9.00 cause it looks cooler
    if (strcmp(FWchar, "9"))
    {
        strcpy(FWchar, "9.00");
    }

    char FWFinal[64];

    // Adding stuff inro FWFinal
    strcpy(FWFinal, "Firmware: ");
    strcat(FWFinal, FWchar);

    // Image that are drawn
    auto logo = new PNG("/app0/assets/images/cd_no_bg.png");

    //Drawing loop
    for (;;)
    {
        // Pointers with text we want to display on screen
        const char *CFWtitle = "CFW Installer";
        const char *CFWdesc = "Finally after years of waiting we made it. \n \n PS4 CFW! Thanks everyone for help! \n \n Coded by Roko191!";
    
        // Check if we are in first loop
        if (i == 0)
        {
            // Displaying FW version as notification
            notifi(NULL, "%s", FWFinal);    
        }

        scene->DrawText((char *)CFWtitle, fontLarge, 150, 400, bgColor, fgColor); // DrawText(text, font, x pos. y pos, background color, foreground color)
        scene->DrawText((char *)CFWdesc , fontSmall, 150, 600, bgColor, fgColor);
        scene->DrawText((char *)FWFinal , fontSmall, 150, 900, bgColor, fgColor);
        scene->DrawText((char *)"(Custom FW ready!)", fontSmall, 450, 900, bgColor, fgColor);
        scene->DrawText((char *)"Press X to start the process", fontSmall, 900, 900, bgColor, fgColor);

        // Drawing the logo 
        logo -> Draw(scene, 1000, 200);

        // Very epic troll moment
        if(controller->XPressed())
        {
            jailbreak();
            sceKernelIccSetBuzzer(6); // Thanks Sony for troll value
            notifi (NULL, "%s", "Sike, you thought! \n Enjoy your beeps!");
            show_dialog(0,"You got epicly trolled! \n Dont trust everything you see online. \n To stop beeping turn off your console and dont open this app again..");
            unjailbreak();
            sceSystemServiceLoadExec("exit", NULL); // Clean exit wow!
        }

        // Submit the frame buffer
        scene->SubmitFlip(frameID);
        scene->FrameWait(frameID);
        

        // Swap to the next buffer
        scene->FrameBufferSwap();
        frameID++;
        i++;
    }
    //Just clenanig up stuff
    //Program should not reach this fase
    
    return 0;
}
