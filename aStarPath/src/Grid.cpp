
#include "Grid.hpp"

XYMEM::XYMEM(){
    this->x=-1;
    this->y=-1;
    this->dist=0;
}

bool Grid::notWall(int x, int y){
    return !(cellGrid[findIndex(x,y)].type == GRIDWALL);
}

double Grid::updateHCost(int x, int y){
    double deltaX = std::fabs(GoalX - x) * 10; 
    double deltaY = std::fabs(GoalY - y) * 10;
    double delta = std::sqrt((deltaX*deltaX)+(deltaY*deltaY));

    cellGrid[findIndex(x,y)].HCost = delta;
    cellGrid[findIndex(x,y)].updateFCost();
    return cellGrid[findIndex(x,y)].HCost;
}

Grid::Grid(){
    this->Width = 0;
    this->Height = 0;
    this->GoalX = 0;
    this->StartX = 0;
    this->GoalY = 0;
    this->StartY = 0;
}

void Grid::setNewSize(int Width, int Height){

    this->Width = Width;
    this->Height = Height;
    
    while(!cellGrid.empty()){
        cellGrid.pop_back();
    }

    for(int i=0; i<Height; i++){
        for(int j=0; j<Width; j++){
            cellGrid.push_back({j,i,EMPTY});
            cameFrom.push_back(Width*Height+1);
        }
    }

    it = cellGrid.begin();
}

int Grid::findIndex(int x, int y){
    return ((y*Width) + x);
}

void Grid::killGrid(){

}

void Grid::setTypeTo(int x, int y, int type){
    cellGrid[findIndex(x,y)] = {x,y,type};
}

int Grid::getCellType(int x, int y){
    int index = findIndex(x,y);
    return cellGrid[index].type;
}

void Grid::resetStart(int x, int y){
    if(!openSet.empty()){
        cellGrid[findIndex(openSet.top().x, openSet.top().y)].inOpenSet = false;
        openSet.pop();
    }
    cellGrid[ findIndex(x,y) ].inOpenSet = true;
    updateHCost(x,y);
    cellGrid[ findIndex(x,y) ].GCost = 0;
    openSet.push( cellGrid[ findIndex(x,y) ] );
}


void Grid::takeStep(){
    if(!pathFound){
        if(!openSet.empty()){
            std::cout << "Open Set Size: " << openSet.size();

            Cell active = openSet.top();
            if (active.x == GoalX && active.y == GoalY){ // PATH FOUND
                currentDrawPath.x = active.x;
                currentDrawPath.y = active.y;
                pathFound = true;
                return;
            }

            int activeIndex = findIndex(active.x, active.y);
            cellGrid[activeIndex].type = CHECKED;

            //FOR EACH NEIGHBOR
            std::vector<XYMEM> neighbors;
            int LX = active.x - 1;
            if(LX < 0)
                LX = Width-1;
            int RX = active.x + 1;
            if (RX == Width)
                RX = 0;
            int TY = active.y - 1;
            if(TY < 0)
                TY = Height-1;
            int BY = active.y + 1;
            if (BY == Height)
                BY = 0;

            updateHCost(LX,TY);
            if(notWall(LX,TY))
                neighbors.push_back(XYMEM(LX,TY,14));

            updateHCost(active.x, TY);
            if(notWall(active.x, TY))
                neighbors.push_back(XYMEM(active.x,TY,10));

            updateHCost(RX, TY);
            if(notWall(RX,TY))
                neighbors.push_back(XYMEM(RX,TY,14));

            updateHCost(LX, active.y);
            if(notWall(LX, active.y))
                neighbors.push_back(XYMEM(LX,active.y,10));

            updateHCost(RX, active.y);
            if(notWall(RX, active.y))
                neighbors.push_back(XYMEM(RX,active.y,10));

            updateHCost(LX, BY);
            if(notWall(LX, BY))
                neighbors.push_back(XYMEM(LX, BY,14));

            updateHCost(active.x, BY);
            if(notWall(active.x, BY))
                neighbors.push_back(XYMEM(active.x,BY,10));

            updateHCost(RX, BY);
            if(notWall(RX, BY))
                neighbors.push_back(XYMEM(RX,BY,14));

            cellGrid[activeIndex].inOpenSet = false;
            std::cout << "X: " << active.x << " Y: " << active.y << " GCOST: " <<active.GCost << " HCOST: "<< active.HCost << std::endl ;
            openSet.pop();
            std::cout << "  (POP)Open Set Size: " << openSet.size() << std::endl;


            while(!neighbors.empty()){
                XYMEM xActive = neighbors.back();

                double newG = cellGrid[activeIndex].GCost + xActive.dist;
                if(newG < cellGrid[findIndex(xActive.x, xActive.y)].GCost){
                    cameFrom[findIndex(xActive.x,xActive.y)] = activeIndex;
                    cellGrid[findIndex(xActive.x,xActive.y)].GCost = newG;
                    cellGrid[findIndex(xActive.x,xActive.y)].FCost = newG + updateHCost(xActive.x, xActive.y);

                    if(!(cellGrid[findIndex(xActive.x,xActive.y)].inOpenSet)){
                        cellGrid[findIndex(xActive.x,xActive.y)].inOpenSet = true;
                        if(cellGrid[findIndex(xActive.x,xActive.y)].type != END)
                            cellGrid[findIndex(xActive.x,xActive.y)].type = UNCHECKED;
                        openSet.push(cellGrid[findIndex(xActive.x,xActive.y)]);
                        std::cout << "(PUSH)Open Set Size: " << openSet.size() << std::endl;
                    }

                }
                neighbors.pop_back();
            }

            return;
        } //if(!openSet.empty())
    } // if(!pathFound)
    
    if(currentDrawPath.x == StartX && currentDrawPath.y == StartY)
        return;

    int cameFromIndex = cameFrom[findIndex(currentDrawPath.x,currentDrawPath.y)];
    Cell nextPath = cellGrid[cameFromIndex];
    currentDrawPath.x = nextPath.x;
    currentDrawPath.y = nextPath.y;
    cellGrid[cameFromIndex].type = PATH;

    //RECONSTRUCT PATH

}
