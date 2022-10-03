
//SDL
#include <SDL2/SDL.h>
//C++ Standard
#include <iostream>

#include "Game.hpp"

//Globals
SDL_Window* window;
SDL_Renderer* renderer;

int CellSize;
int G_Width;
int G_Height;

bool run;
bool pathfind = false;

bool STARTSET = false;
bool ENDSET = false;

int drawMode;

Grid Ggrid;

//Entry Point
int main(void){
    run = true;
    drawMode = 0;

    getGridStartingConditions();

    Ggrid.setNewSize(G_Width, G_Height);

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        std::cout << "SDL FAILED TO INITIALIZE. ERROR: " << SDL_GetError();

    window = SDL_CreateWindow("A_Star Path Finder", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (G_Width*CellSize), (G_Height*CellSize), SDL_WINDOW_SHOWN);
    if (window == NULL)
        std::cout << "SDL WINDOW FAILED TO START. ERROR: " << SDL_GetError();
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int windowREFRATE = getRefRate(); std::cout << "\n" << windowREFRATE << std::endl;
    const float timestep = 0.033f;
    float accumulator = 0.0f;

    float currentTime = (SDL_GetTicks64() * 0.001f); // Time in seconds 

    std::cout << "\n Starting Current Time: " << currentTime << std::endl;


    while(run){
        int startTicks = SDL_GetTicks();
        
        float newTime = (SDL_GetTicks64() * 0.001f);
        float frameTime = newTime - currentTime; // Time Frame Took to process

        if (frameTime > 0.25f)
            frameTime = 0.25f;

        currentTime = newTime;
        accumulator += frameTime;
        
        while(accumulator >= timestep){
            inputHandle();

            if(pathfind)
                updateState();

            accumulator -= timestep;
        }

        float alpha = accumulator / timestep;
        drawNewState(&alpha);

        int frameTicks = SDL_GetTicks() - startTicks;
        if(frameTicks < (1000/windowREFRATE) ){
            SDL_Delay((1000/windowREFRATE) - frameTicks);
        }
    }

    Ggrid.killGrid();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


void inputHandle(){

    SDL_Event event;
    int X, Y;
    Uint32 buttons;

    while (SDL_PollEvent(&event)){

        switch(event.type){

            case(SDL_QUIT):
                run = false;
                break;

            case(SDL_MOUSEBUTTONDOWN):

                buttons = SDL_GetMouseState(&X, &Y);

                if((buttons & SDL_BUTTON_LEFT) > 0){

                    int x = floor(X/CellSize);
                    int y = floor(Y/CellSize);
                
                    switch(drawMode){

                        case(0):

                            switch(Ggrid.getCellType(x,y)){
                                case(Grid::START):
                                    STARTSET = false;
                                    break;
                                case(Grid::END):
                                    ENDSET = false;
                                    break;
                            };

                            Ggrid.setTypeTo(x,y,Grid::GRIDWALL);
                            break;

                        case(1):

                            if(STARTSET){
                                std::cout << "Start has already been set" << std::endl;
                                break;
                            }

                            Ggrid.setTypeTo(x,y,Grid::START);
                            Ggrid.resetStart(x,y);
                            Ggrid.StartX = x;
                            Ggrid.StartY = y;
                            STARTSET = true;
                            break;

                        case(2):

                            if(ENDSET){
                                std::cout << "END has already been set" << std::endl;
                                break;
                            }

                            Ggrid.setTypeTo(x,y,Grid::END);
                            Ggrid.GoalX = x;
                            Ggrid.GoalY = y;
                            ENDSET = true;
                            break;

                        case(3):

                            switch(Ggrid.getCellType(x,y)){
                                case(Grid::START):
                                    STARTSET = false;
                                    break;
                                case(Grid::END):
                                    ENDSET = false;
                                    break;
                            };

                            Ggrid.setTypeTo(x,y,Grid::EMPTY);
                            break;
                    };


                    SDL_RenderPresent(renderer);
                }
                break;

            case(SDL_KEYDOWN):
                const Uint8 *keystates = SDL_GetKeyboardState(NULL);
                if(keystates[SDL_SCANCODE_W]) // Set Wall
                    drawMode = 0;
                if(keystates[SDL_SCANCODE_S]) // Set Start
                    drawMode = 1;
                if(keystates[SDL_SCANCODE_G]) // Set Goal
                    drawMode = 2;
                if(keystates[SDL_SCANCODE_E]) // Erase
                    drawMode = 3;
                if(keystates[SDL_SCANCODE_B]){ // Stop Wrap-Around
                    for(int i=0; i< Ggrid.Width; i++){
                        Ggrid.cellGrid[i].type = Grid::GRIDWALL;
                    }
                    for(int i=0; i< Ggrid.Height; i++){
                        Ggrid.cellGrid[i * Ggrid.Width].type = Grid::GRIDWALL;
                    }
                }
                if(keystates[SDL_SCANCODE_R]){
                    for(unsigned long int i=0; i < Ggrid.cellGrid.size(); i++){
                        if(rand() % 3 == 0){
                            Ggrid.cellGrid[i].type = Grid::GRIDWALL;
                        }
                    }
                }
                if(keystates[SDL_SCANCODE_BACKSPACE]){

                    std::cout << "\nRESETING GRID COMPLETELY\n" << std::endl;
                    Ggrid.GoalX = Ggrid.GoalY = Ggrid.StartX = Ggrid.StartY = 0;
                    Ggrid.pathFound = false;
                    int finalIndex = Ggrid.findIndex(G_Width-1,G_Height-1);
                    for(int i=0;;){
                        if(i > finalIndex)
                            break;
                        Ggrid.cellGrid[i].type = Grid::EMPTY;
                        Ggrid.cellGrid[i].GCost = __DBL_MAX__;
                        Ggrid.cellGrid[i].inOpenSet = false;
                        i++;
                    }
                    while(!Ggrid.openSet.empty()){
                        Ggrid.openSet.pop();
                    }
                    Ggrid.currentDrawPath = {-1,-1,0};
                    STARTSET = false;
                    ENDSET = false;

                }
                if(keystates[SDL_SCANCODE_SPACE]){
                    if(pathfind){
                        pathfind = false;
                        continue;
                    }
                    if(!STARTSET){
                        std::cout << "START NOT SET\n";
                        continue;
                    }
                    if(!ENDSET){
                        std::cout << "END NOT SET\n";
                        continue;
                    }   
                    pathfind = true;
                }

                break;
        };
    }
    
}

void updateState(){
    Ggrid.takeStep();
}

void drawNewState(const float *alpha){

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    for(int i=0; i<G_Height; i++){
        for(int j=0; j<G_Width; j++){

            struct Cell activeCell = Ggrid.cellGrid.at(Ggrid.findIndex(j,i));

            SDL_Rect currentRect;

            currentRect.h = currentRect.w = CellSize;
            currentRect.x = activeCell.x * CellSize;
            currentRect.y = activeCell.y * CellSize;

            bool emp = false;
            switch(activeCell.type){
                case(Grid::EMPTY): 
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    emp = true;
                    break;
                case(Grid::START): 
                    SDL_SetRenderDrawColor(renderer, 20, 145, 90, 255);
                    break;
                case(Grid::END): 
                    SDL_SetRenderDrawColor(renderer, 250, 15, 15, 255);
                    break;
                case(Grid::GRIDWALL): 
                    SDL_SetRenderDrawColor(renderer, 45, 40, 40, 255);
                    break;
                case(Grid::CHECKED): 
                    SDL_SetRenderDrawColor(renderer, 102, 255, 102, 255);
                    break;
                case(Grid::UNCHECKED): 
                    SDL_SetRenderDrawColor(renderer, 102, 178, 255, 255);
                    break;
                case(Grid::PATH): 
                    SDL_SetRenderDrawColor(renderer, 255, 204, 179, 255);

                    if (activeCell.y == Ggrid.GoalY){
                        if (activeCell.x == Ggrid.GoalX)
                            pathfind = false;
                    }
                    
                    break;
            };

            if(emp){
                SDL_RenderDrawRect(renderer, &currentRect);
            } else {
                SDL_RenderFillRect(renderer, &currentRect);
            }

        }
    }

    if(!pathfind){ // Draw Pause Symbol

        SDL_Rect pause = {(int)(Ggrid.Height * CellSize * .02), (int)(Ggrid.Height * CellSize * .02), (int)((Ggrid.Height * CellSize) * .05), (int)((Ggrid.Height * CellSize) * .05)};
        SDL_Rect line1 = { (int)(pause.x+(pause.w * .20)), (int)(pause.y+(pause.h * .17)), (int)(pause.w*.2), (int)(pause.h*.75)};
        SDL_Rect line2 = { (int)(line1.x+(pause.w * .35)), (int)(pause.y+(pause.h * .17)), (int)(pause.w*.2), (int)(pause.h*.75)};
        
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 50);
        SDL_RenderDrawRect(renderer, &pause);
        SDL_RenderFillRect(renderer, &pause);

        SDL_SetRenderDrawColor(renderer, 120, 120, 120, 100);
        SDL_RenderFillRect(renderer, &line1);
        SDL_RenderFillRect(renderer, &line2);

    }


    SDL_RenderPresent(renderer);
    
}

int getRefRate(){
    int displayIndex = SDL_GetWindowDisplayIndex(window);
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(displayIndex, 0,&mode);

    return mode.refresh_rate;
}

void getGridStartingConditions(){

    askFor("\nEnter (in pixels) the Cell Size: ", &CellSize, "The Cell Size\n");
    askFor("\nEnter the width of the grid (in # of cells): ", &G_Width, "The Grid Width\n");
    askFor("\nEnter the height of the grid (in # of cells): ", &G_Height, "The Grid Height\n");
    
}

void askFor(const char* message, int* variable, const char* varname){
    bool isInputInCorrect;
    do {
        std::cout << message;
        std::cin >> *variable;

        isInputInCorrect = std::cin.fail();
        if(isInputInCorrect){
            std::cout << "Enter an appropiate input for ";
            std::cout << varname;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }

    } while (isInputInCorrect);

    std::cout << *variable << " " << std::endl;
}