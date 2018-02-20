#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <algorithm>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

std::string choices[] = { "  +---------------------------------------+",
                    "  +-           JANNIS RPi MENU           -+",
                    "  +---------------------------------------+",
                    "09| Exit to Console                       |",
                    "  +                                       +",
                    "01|  APT UPDATE UPGRADE                   |",
					"02|                                       |",
					"03|                                       |",
                    "07|                                       |",
                    "08|                                       |",
                    "05|                                       |",
                    "06|                                       |",
                    "  +                                       |",
                    "04|                                       |",
                    "10|                                       |",
                    "  +                                       |",
		  };

int main(){
 ITEM **myItems;
 ITEM *currentItem;
 MENU *myMenu;
 int numberChoices = ARRAY_SIZE(choices);

 system("clear");

 initscr();
 cbreak();
 noecho();

 start_color();

 init_pair(1, COLOR_RED, COLOR_BLACK);
 init_pair(2, COLOR_WHITE, COLOR_BLACK);
 init_pair(3, COLOR_CYAN, COLOR_BLACK);
 init_pair(4, COLOR_YELLOW, COLOR_BLACK);
 init_pair(5, COLOR_GREEN, COLOR_BLACK);

 attron(COLOR_PAIR(2));

 myItems = (ITEM **)calloc(numberChoices + 1, sizeof(ITEM *));

 for(int i = 0; i < numberChoices; i++){
  myItems[i] = new_item(choices[i].c_str(), "");
 }
 myItems[numberChoices] = (ITEM *)NULL;

 keypad(stdscr, true);

 myMenu = new_menu((ITEM **)myItems);

 post_menu(myMenu);

 mvprintw(16,0,"   +---------------------------------------+");

 int xpostemp = 2;
 int ypostemp = 46;
 int menulenght = 16;
 int menuwidth = 36;

 std::string spacers = "+";

 for(int x = 0; x <= menuwidth; x++)
  spacers += "-";
 spacers += "+";

 mvprintw(xpostemp-2,ypostemp-2,spacers.c_str());
 mvprintw(xpostemp+menulenght,ypostemp-2,spacers.c_str());

 for(int i = 0; i <= menulenght; i++){
  mvprintw(xpostemp-1+i,ypostemp-2,"|");
  mvprintw(xpostemp-1+i,ypostemp+menuwidth,"|");
 }

 attron(A_BOLD);

 mvprintw(xpostemp-1,ypostemp,"Temperatures:");
 mvprintw(xpostemp+8,ypostemp,"ClockSpeed:");
 mvprintw(xpostemp+11,ypostemp,"Average Load:");
 mvprintw(xpostemp+14,ypostemp,"IP-Address:");

 attron(COLOR_PAIR(1));

 std::string load;
 std::ifstream loadFile;
 loadFile.open("/proc/loadavg");
 getline(loadFile,load);
 loadFile.close();

 std::string temprpi, tempopi, tempopz, tempeee;
 std::string tempx;

 temprpi = "RaspberryPi   [";
 tempx += exec("vcgencmd measure_temp | cut -c 6-");
 tempx.erase(std::remove(tempx.begin(), tempx.end(), '\n'), tempx.end());
 temprpi += tempx;
 tempx = "";
 temprpi += "]";

 tempopi += "OrangePi      [";
 tempx += exec("cat /home/pi/DEV/FanScript/opitemp");
 tempx.erase(std::remove(tempx.begin(), tempx.end(), '\n'), tempx.end());
 tempopi += tempx;
 tempx = "";
 tempopi += ".0'C]";

 tempopz += "OrangePiZero  [";
 tempx += exec("cat /home/pi/DEV/FanScript/opiztemp");
 tempx.erase(std::remove(tempx.begin(), tempx.end(), '\n'), tempx.end());
 tempopz += tempx;
 tempx = "";
 tempopz += ".0'C]";

 tempeee += "EEEbtuntu     [";
 tempx += exec("cat /home/pi/DEV/FanScript/eeetemp");
 tempx.erase(std::remove(tempx.begin(), tempx.end(), '\n'), tempx.end());
 tempeee += tempx;
 tempx = "";
 tempeee += "'C]";

 //temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());

 std::string clock;
 std::ifstream clockFile;
 clockFile.open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
 getline(clockFile,clock);
 clockFile.close();

 clock = std::to_string(atoi(clock.c_str()) / 1000);
 clock += " MHz  (FAN: ";
 clock += exec("gpio -g read 23");
 clock += " | FORCE: ";
 clock += exec("cat /home/pi/DEV/FanScript/force");
 clock += ")";

 clock.erase(std::remove(clock.begin(), clock.end(), '\n'), clock.end());

 std::string uptime;
 uptime = "";
 uptime += exec("uptime -p | cut -c 4-");
 uptime += "";

 uptime.erase(std::remove(uptime.begin(), uptime.end(), '\n'), uptime.end());

 std::string ip = exec("ifconfig eth1 | grep 'inet Adresse:' | cut -d: -f2 | cut -c 1-15");
 ip.erase(std::remove(ip.begin(), ip.end(), '\n'), ip.end());

 mvprintw(xpostemp+9,ypostemp,clock.c_str());
 mvprintw(xpostemp+12,ypostemp,load.c_str());
 mvprintw(xpostemp+15,ypostemp,ip.c_str());

 attron(COLOR_PAIR(5));
 mvprintw(xpostemp,ypostemp,temprpi.c_str());
 mvprintw(xpostemp+1,ypostemp,tempopi.c_str());
 mvprintw(xpostemp+2,ypostemp,tempopz.c_str());
 mvprintw(xpostemp+3,ypostemp,tempeee.c_str());

 attron(COLOR_PAIR(4));
 mvprintw(xpostemp+6,ypostemp,uptime.c_str());

 attron(COLOR_PAIR(2));
 mvprintw(xpostemp+5,ypostemp,"Uptime:");

 attron(COLOR_PAIR(3));

 mvprintw(19,3,"       _____    _   ___   ___________    ____  ____  ____");
 mvprintw(20,3,"      / /   |  / | / / | / /  _/ ___/   / __ \\/ __ \\/  _/");
 mvprintw(21,3," __  / / /| | /  |/ /  |/ // / \\__ \\   / /_/ / /_/ // /  ");
 mvprintw(22,3,"/ /_/ / ___ |/ /|  / /|  // / ___/ /  / _, _/ ____// /   ");
 mvprintw(23,3,"\\____/_/  |_/_/ |_/_/ |_/___//____/  /_/ |_/_/   /___/   ");

 attron(COLOR_PAIR(2));
 move(0,0);
 refresh();

 int c;
 bool stop = false;
 bool fullstop = false;

 while((c = getch()) != KEY_F(1)){
  std::string s;
  switch(c){
   case KEY_DOWN:
    menu_driver(myMenu, REQ_DOWN_ITEM);
   break;
   case KEY_UP:
    menu_driver(myMenu, REQ_UP_ITEM);
   break;
   case 10:
    std::string itemNumber(item_name(current_item(myMenu)), 2);
    clrtoeol();
    //mvprintw(20, 0, itemNumber.c_str());
    switch(atoi(itemNumber.c_str())){
     case 1:
     exec("apt update | apt upgrade");
      //unpost_menu(myMenu);
      //free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      system("sshpass -p x ssh -o StrictHostKeyChecking=no ");
      stop = true;
     break;
     case 2:
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      system("sshpass -p x ssh -o StrictHostKeyChecking=no ");
      stop = true;
     break;
     case 3:
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      system("sshpass -p x ssh -o StrictHostKeyChecking=no ");
      stop = true;
     break;
     case 4:
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      s = exec("cat /home/pi/DEV/FanScript/force");
      if(atoi(s.c_str()) == 1){
       system("echo 0 > /home/pi/DEV/FanScript/force");
       system("gpio -g write 23 0");
      }
      else {
       system("echo 1 > /home/pi/DEV/FanScript/force");
       system("gpio -g write 23 1");
      }
      stop = true;
     break;
     case 5:
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      system("sshpass -p x ssh -o StrictHostKeyChecking=no ");
      stop = true;     break;
     case 6:
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      system("sshpass -p x ssh -o StrictHostKeyChecking=no ");
      stop = true;     break;
     case 7:
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      system("sshpass -p x ssh -o StrictHostKeyChecking=no ");
      stop = true;
     break;
     case 8:
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      system("sshpass -p x ssh -o StrictHostKeyChecking=no ");
      stop = true;
     break;
     case 9:
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      fullstop = true;
     break;
     case 10:
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      system("sudo /home/pi/DEV/FanScript/refresh.sh");
      stop = true;
     break;

     default:
      system("clear");
      unpost_menu(myMenu);
      free_menu(myMenu);
      for(int i = 0; i < numberChoices; ++i)
       free_item(myItems[i]);
      endwin();
      stop = true;
     break;
    }
    pos_menu_cursor(myMenu);
   break;
  }
  if(fullstop || stop)
   break;
 }

 if(!fullstop)
  system("/home/pi/DEV/HomeMenu/HomeMenu");

 return 0;
}
