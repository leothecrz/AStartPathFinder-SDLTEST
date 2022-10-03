
#pragma once
#include <vector>
#include <queue>
#include <iostream>
#include <math.h>

struct XYMEM{
    int x;
    int y;
    int dist;
    XYMEM();
    XYMEM(int x,int y, int dist):x(x), y(y), dist(dist) 
    {}
};

struct Cell{
        int x;
        int y;
        int type;

        double GCost = __DBL_MAX__; // COST from start
        double HCost = __DBL_MAX__; // COST to end
        double FCost = __DBL_MAX__; //TOTALCOST

        bool inOpenSet = false;

        void updateFCost(){
            FCost = (GCost + HCost);
        }
        
};

class CompareCost {

    public:
        bool operator() (Cell& a, Cell& b){
            if (a.FCost > b.FCost)
                return true;
            return false;
        }
};

class Grid{

    public:
        
        std::vector<Cell> cellGrid;
        std::vector<Cell>::iterator it; 
        int Width;
        int Height;

        int StartX;
        int StartY;

        int GoalX;
        int GoalY;

        bool pathFound = false;

        std::vector<int> cameFrom;
        std::priority_queue<Cell, std::vector<Cell>, CompareCost> openSet;

        XYMEM currentDrawPath;

        enum CellTypes{
            EMPTY,
            START,
            END,
            GRIDWALL,
            UNCHECKED,
            CHECKED,
            PATH
        };

        Grid();
        void killGrid();
        void setNewSize(int Width, int Height);

        int findIndex(int x, int y);
        void setTypeTo(int x, int y, int type);
        int getCellType(int x, int y);
        void takeStep();

        void resetStart(int x, int y);

        double updateHCost(int x, int y);
        bool notWall(int x, int y);

};

