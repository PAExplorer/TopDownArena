/*
    LeRPG -v0.0
    Jeff Dickson
    Aug 29 2022
*/

#include <iostream>
#include <vector>
#include <Windows.h>
#include <thread>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <time.h>

#include "DungeonTemplates.h"

using namespace std;

int nGameState = 0;
bool bGameOver = false;
int nTerminalWidth = 80;
int nTerminalHeight = 25;
int nTicToc = 0;
int nHeroX = 40;
int nHeroY = 12;
int nScore = 0;
int nArrowDir = 0; //direction and activation of arrow if 0 arrow is inactive if 4 5 6 7 arrow is moving left up right or down
int nNumProjectiles = 0;
int nArrowCoordX = nTerminalWidth;
int nArrowCoordY = nTerminalHeight;
int nDamageSpotX = 0;
int nDamageSpotY = 0;
bool bEnemyAlive = false;
int nEnemyX = 0;
int nEnemyY = 0;

int MoveDirX(bool arrowIn[]) {
    if (arrowIn[0] == true) { 
        return 1; 
    }
    if (arrowIn[2] == true) {
        return -1;
    }
    return 0;
}
int MoveDirY(bool arrowIn[]) {
    if (arrowIn[1] == true) {
        return 1;
    }
    if (arrowIn[3] == true) {
        return -1;
    }
    return 0;
}

bool CheckWallOverlap(int pX, int pY, int termWidth, int termHeight) {
    if (pX > termWidth - 1 || pX < 1 || pY > termHeight || pY < 1) {
        return true;
    }
    return false;
}
bool CheckEnemyCollision(int pX, int pY, int eX, int eY) {
    if (pX < eX + 2 && pX > eX - 2) {
        if (pY < eY + 2 && pY > eY - 2) {
            return true;
        }
    }
    return false;
}

class projectile {
public:
    int direction;

};

int main()
{
    //Screen buffer, this creates an array the size of our terminal screen
    wchar_t* screen = new wchar_t[nTerminalWidth * nTerminalHeight];
    for (int n = 0; n < nTerminalWidth * nTerminalHeight; n++) {
        screen[n] = L' ';
    }
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    string dungeonWalls[2];
    {
        dungeonWalls[0].append("................................................................................");
        dungeonWalls[0].append("................................................................................");
        dungeonWalls[0].append("................................................................................");
        dungeonWalls[0].append("................................................................................");
        dungeonWalls[0].append(".......---.....-----...---....--.--..--..--...---..-----...---...--..--.........");
        dungeonWalls[0].append("......|...|...|..---..|...\\..|..|..||..\\|..|./...\\|..---../...\\.|..\\|..|........");
        dungeonWalls[0].append("......|...|...|..---..|.|\\.|.|..|..||......||.....|..---.|..|..||......|........");
        dungeonWalls[0].append("......|....--.|..---..|.|/.|.|..|..||......||..---|..---.|..|..||......|........");
        dungeonWalls[0].append("......|......||.....|.|..../..\\.../.|..|\\..|.\\.../|.....|.\\.../.|..|\\..|........");
        dungeonWalls[0].append(".......------..-----...---.....---...--..--...---..-----...---...--..--.........");
        dungeonWalls[0].append("................................................................................");
        dungeonWalls[0].append("...............................+................................................");
        dungeonWalls[0].append("..+.............................................................................");
        dungeonWalls[0].append("...............................................................+................");
        dungeonWalls[0].append("................................................................................");
        dungeonWalls[0].append("..............+.................................................................");
        dungeonWalls[0].append("................................................................................");
        dungeonWalls[0].append("...................................................................+............");
        dungeonWalls[0].append("................................................................................");
        dungeonWalls[0].append(".....+..............................__..............+...........................");
        dungeonWalls[0].append("...................................|..|.........................................");
        dungeonWalls[0].append("--------------------------------------------------------------------------------");
        dungeonWalls[0].append("................................../.../.........................................");
        dungeonWalls[0].append("................................./.../..........................................");
        dungeonWalls[0].append("................................/....\\........................................................");

        dungeonWalls[1].append("................................................................................");
        dungeonWalls[1].append(".==============================================================================.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".|............................................................................|.");
        dungeonWalls[1].append(".==============================================================================.");
        dungeonWalls[1].append("................................................................................");
    }

    projectile arrow;
    arrow.direction = 0;
    
    srand(time(0));



    bool bKey[8]; //this needs to be as long as there are keys you want mapped

    while (!bGameOver) { //primnary loop
        this_thread::sleep_for(50ms);
        if (nTicToc > 2) {
            nTicToc = 0;
        }
        else {
            nTicToc++;
        }

        //====================Retrive Inputs====================//
        for (int k = 0; k < 8; k++) { //loops through the bkey array and activates element related to the given hexidecimal values
            auto keyHexes = "\x25\x26\x27\x28\x41\x57\x44\x53"; //left up right down a w d s
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)(keyHexes[k]))) != 0;
        }

        switch (nGameState) {
        case 0:
            for (int x = 0; x < nTerminalWidth; ++x) {
                for (int y = 0; y < nTerminalHeight; ++y) {

                    if (dungeonWalls[0][(y * nTerminalWidth) + x] != '.') {
                        screen[(y)*nTerminalWidth + (x)] = dungeonWalls[0][(y * nTerminalWidth) + x];
                    }
                    else {
                        screen[(y)*nTerminalWidth + (x)] = ' ';
                    }
                }
            }

            for (int k = 0; k < 8; k++) {
                if (bKey[k] == true) {
                    nGameState = 1;
                }
            }

            break;
        case 1:
                //====================Game Logic====================//
                //interpret arrow keys, allows for 8dir movement
                if (nTicToc == 0 || nTicToc == 2) {
                    if ((nHeroX - MoveDirX(bKey)) < nTerminalWidth - 2 && (nHeroX - MoveDirX(bKey)) > 2) {
                        nHeroX -= MoveDirX(bKey);
                    }
                    if ((nHeroY - MoveDirY(bKey)) < nTerminalHeight - 2 && (nHeroY - MoveDirY(bKey)) > 2) {
                        nHeroY -= MoveDirY(bKey);
                    }
                }

                //interpret input from arrow shooter
                for (int i = 4; i < 8; i++) {
                    if (bKey[i] == true && nArrowDir == 0) {
                        nArrowCoordX = nHeroX;
                        nArrowCoordY = nHeroY;
                        nArrowDir = i;
                    }
                }

                //Arrow Logic
                if (nArrowDir != 1) {
                    switch (nArrowDir) {
                    case 4: //left
                        nArrowCoordX--;
                        break;
                    case 5: //up
                        nArrowCoordY--;
                        break;
                    case 6: //right
                        nArrowCoordX++;
                        break;
                    case 7: //down
                        nArrowCoordY++;
                        break;
                    }

                    //Check if new coordinates create collision with walls, if so delete the projectile
                    if (CheckWallOverlap(nArrowCoordX, nArrowCoordY, nTerminalWidth, nTerminalHeight) == true) {
                        nArrowDir = 0;
                    }
                    if (CheckEnemyCollision(nArrowCoordX, nArrowCoordY, nEnemyX, nEnemyY)) { //if arrow coordinates match enemy kill enemy and make a new one
                        nArrowDir = 0;
                        bEnemyAlive = false;
                        nScore += 10;
                        nArrowCoordX = nTerminalWidth;
                        nArrowCoordY = nTerminalHeight;

                    }
                }
                //Enemy Logic

                if (bEnemyAlive == false) { //create a new enemy if one doesn't exist
                    nEnemyX = ((rand() % ((nTerminalWidth - 4)) + 2));
                    nEnemyY = ((rand() % ((nTerminalHeight - 4)) + 2));
                    bEnemyAlive = true;
                }
                else {
                    if (nTicToc == 3) {
                        if (nEnemyX < nHeroX) {
                            nEnemyX++;
                        }
                        if (nEnemyX > nHeroX) {
                            nEnemyX--;
                        }
                        if (nEnemyY < nHeroY) {
                            nEnemyY++;
                        }
                        if (nEnemyY > nHeroY) {
                            nEnemyY--;
                        }
                    }
                }

                //if enemy collides with player trigger a game over
                if (CheckEnemyCollision(nEnemyX, nEnemyY, nHeroX, nHeroY)) {
                    bGameOver = true;
                }






                //====================Write Characters to Screen====================//
                //Overwrite Old Sprites with blank
                for (int n = 0; n < nTerminalWidth * nTerminalHeight; n++) {
                    screen[n] = L' ';
                }

                //Draw dungeon walls (This is here because moving forward new sections will be available)
                for (int x = 0; x < nTerminalWidth; ++x) {
                    for (int y = 0; y < nTerminalHeight; ++y) {

                        if (dungeonWalls[1][(y * nTerminalWidth) + x] != '.') {
                            screen[(y)*nTerminalWidth + (x)] = dungeonWalls[1][(y * nTerminalWidth) + x];
                        }
                        else {
                            screen[(y)*nTerminalWidth + (x)] = ' ';
                        }
                    }
                }

                //Draw Projectile
                if (nArrowDir != 0) {
                    screen[(nArrowCoordY)*nTerminalWidth + (nArrowCoordX)] = '+';
                }
                //Draw Current Location of Hero
                screen[(nHeroY)*nTerminalWidth + (nHeroX)] = '&';

                //Draw Enemy
                if (bEnemyAlive == true) {
                    screen[(nEnemyY)*nTerminalWidth + (nEnemyX)] = 'X';
                }

                //Draw Score
                swprintf_s(&screen[nTerminalWidth * (nTerminalHeight - 1) + 2], 16, L"SCORE: %8d", nScore);

                
                break;
        }
        WriteConsoleOutputCharacter(hConsole, screen, nTerminalWidth* nTerminalHeight, { 0,0 }, & dwBytesWritten);

    }
    CloseHandle(hConsole);
    cout << "Game Over!! Score: " << nScore << endl;
    this_thread::sleep_for(500ms);
    system("pause");

    return 0;
}
