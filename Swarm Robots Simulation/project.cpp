#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>                                                                             
#include <chrono>

using namespace std;

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <queue>
#include <filesystem>
#include <ctime>

// Forward declarations so Robot::move can take world context
class Grid;
class Resource;
class Obstacle;


// Convert integer to string (for old Dev-C++)
string numberToString(int number)
{
    stringstream ss;
    ss << number;
    return ss.str();
}



// ===============================
// BASE CLASS: ROBOT
// ===============================

class Robot
{
protected:

    int id;
    int x;
    int y;
    string status;
    char symbol;
    int energy;
    pair<int,int> homePos;
    bool returningHome;


public:

    Robot(int id, int x, int y, char symbol)
    {
        this->id = id;
        this->x = x;
        this->y = y;
        this->symbol = symbol;
        status = "Idle";
        energy = 100;
        homePos = {0, 0};
        returningHome = false;
    }


    virtual ~Robot()
    {

    }


    virtual void move(Grid& grid, vector<Resource>& resources, vector<Obstacle>& obstacles) = 0;


    void display()
    {
        cout << "Robot ID: " << id << endl;
        cout << "Position: (" << x << "," << y << ")" << endl;
        cout << "Status: " << status << endl;
    }


    int getID()
    {
        return id;
    }

    int getEnergy() { return energy; }
    void setEnergy(int e) { energy = std::max(0, std::min(100,e)); }
    void drainEnergy(int amt){ setEnergy(energy - amt); }

    void setHome(int hx, int hy) { homePos = {hx, hy}; }
    pair<int,int> getHome() { return homePos; }
    bool isReturningHome() { return returningHome; }
    void setReturningHome(bool v) { returningHome = v; }


    int getX()
    {
        return x;
    }


    int getY()
    {
        return y;
    }


    char getSymbol()
    {
        return symbol;
    }


    string getStatus()
    {
        return status;
    }


    void setPosition(int newX, int newY)
    {
        x = newX;
        y = newY;
    }


    void setStatus(string newStatus)
    {
        status = newStatus;
    }

};
// ===============================
// COLLECTOR ROBOT CLASS
// ===============================

class CollectorRobot : public Robot
{

public:

    CollectorRobot(int id, int x, int y)
        : Robot(id, x, y, 'C')
    {

    }
    void move(Grid& grid, vector<Resource>& resources, vector<Obstacle>& obstacles);

};




// ===============================
// EXPLORER ROBOT CLASS
// ===============================

class ExplorerRobot : public Robot
{

public:

    ExplorerRobot(int id, int x, int y)
        : Robot(id, x, y, 'E')
    {

    }



    void move(Grid& grid, vector<Resource>& resources, vector<Obstacle>& obstacles);

};
// ===============================
// RESOURCE CLASS
// ===============================

class Resource
{

private:

    int x;
    int y;
    bool collected;
    bool carried;
    bool delivered;
    int carrierID;


public:

    Resource(int x, int y)
    {
        this->x = x;
        this->y = y;
        collected = false;
        carried = false;
        delivered = false;
        carrierID = -1;
    }



    int getX()
    {
        return x;
    }



    int getY()
    {
        return y;
    }



    bool isCollected()
    {
        return collected;
    }

    bool isCarried()
    {
        return carried;
    }

    bool isDelivered()
    {
        return delivered;
    }

    void setPosition(int nx, int ny)
    {
        x = nx; y = ny;
    }

    void setCarrier(int id)
    {
        carrierID = id;
    }

    int getCarrier()
    {
        return carrierID;
    }



    void collect()
    {
        collected = true;
        carried = true;
    }

    void deliver()
    {
        carried = false;
        delivered = true;
        collected = true;
        carrierID = -1;
    }



    void display()
    {
        if(!collected)
        {
            cout << "Resource at ("
                 << x << ","
                 << y << ")"
                 << endl;
        }
    }

};





// ===============================
// OBSTACLE CLASS
// ===============================

class Obstacle
{

private:

    int x;
    int y;


public:

    Obstacle(int x, int y)
    {
        this->x = x;
        this->y = y;
    }



    int getX()
    {
        return x;
    }



    int getY()
    {
        return y;
    }



    void display()
    {
        cout << "Obstacle at ("
             << x << ","
             << y << ")"
             << endl;
    }

};
// ===============================
// GRID CLASS
// ===============================

class Grid
{

private:

    int size;



public:

    Grid(int size = 15)
    {
        this->size = size;
    }




    // Check if position is inside grid

    bool isInside(int x, int y)
    {

        if(x >= 0 && x < size &&
           y >= 0 && y < size)
        {
            return true;
        }

        return false;

    }





    // Check obstacle location

    bool hasObstacle(vector<Obstacle>& obstacles, int x, int y)
    {

        for(int i = 0; i < obstacles.size(); i++)
        {

            if(obstacles[i].getX() == x &&
               obstacles[i].getY() == y)
            {
                return true;
            }

        }


        return false;

    }

    // Find next step from (sx,sy) towards (tx,ty) avoiding obstacles using BFS
    // Returns pair(nextX,nextY). If no path, returns original (sx,sy).
    pair<int,int> nextStep(int sx, int sy, int tx, int ty, vector<Obstacle>& obstacles)
    {
        if(sx == tx && sy == ty) return {sx,sy};

        vector<vector<bool>> blocked(size, vector<bool>(size,false));
        for(size_t i=0;i<obstacles.size();++i) blocked[obstacles[i].getX()][obstacles[i].getY()] = true;

        vector<vector<bool>> vis(size, vector<bool>(size,false));
        vector<vector<pair<int,int>>> parent(size, vector<pair<int,int>>(size, {-1,-1}));

        queue<pair<int,int>>q;
        q.push({sx,sy}); vis[sx][sy]=true;

        int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};

        bool found=false;
        while(!q.empty())
        {
            auto p = q.front(); q.pop();
            if(p.first==tx && p.second==ty){ found=true; break; }
            for(int d=0;d<4;d++){
                int nx=p.first+dirs[d][0];
                int ny=p.second+dirs[d][1];
                if(nx<0||ny<0||nx>=size||ny>=size) continue;
                if(blocked[nx][ny]) continue;
                if(!vis[nx][ny]){ vis[nx][ny]=true; parent[nx][ny]=p; q.push({nx,ny}); }
            }
        }

        if(!found) return {sx,sy};

        // backtrack from target to source to find next step
        pair<int,int> cur={tx,ty};
        pair<int,int> prev=parent[cur.first][cur.second];
        while(!(prev.first==sx && prev.second==sy) && !(prev.first==-1 && prev.second==-1)){
            cur=prev; prev=parent[cur.first][cur.second];
        }
        if(prev.first==-1) return {sx,sy};
        return cur;
    }






    // Display Grid

    void display(vector<Robot*>& robots,
                 vector<Resource>& resources,
                 vector<Obstacle>& obstacles)
    {


        cout << endl;
        cout << "========== SWARM ROBOTICS GRID =========="
             << endl;
        cout << endl;



        for(int i = 0; i < size; i++)
        {

            for(int j = 0; j < size; j++)
            {

                bool printed = false;




                // Display Robots

                for(int r = 0; r < robots.size(); r++)
                {

                    if(robots[r]->getX() == i &&
                       robots[r]->getY() == j)
                    {

                        cout << robots[r]->getSymbol()
                             << " ";

                        printed = true;
                        break;

                    }

                }





                // Display Resources

                if(!printed)
                {

                    for(int r = 0; r < resources.size(); r++)
                    {

                        if(resources[r].getX() == i &&
                           resources[r].getY() == j &&
                           !resources[r].isCollected())
                        {

                            cout << "* ";

                            printed = true;
                            break;

                        }

                    }

                }





                // Display Obstacles

                if(!printed)
                {

                    for(int o = 0; o < obstacles.size(); o++)
                    {

                        if(obstacles[o].getX() == i &&
                           obstacles[o].getY() == j)
                        {

                            cout << "# ";

                            printed = true;
                            break;

                        }

                    }

                }





                // Empty Cell

                if(!printed)
                {
                    cout << ". ";
                }


            }


            cout << endl;

        }



        cout << endl;

    }

};
// CollectorRobot move implementation moved after Resource/Obstacle/Grid definitions

void CollectorRobot::move(Grid& grid, vector<Resource>& resources, vector<Obstacle>& obstacles)
{
    // Check if all resources are collected
    bool allDone = true;
    for(size_t i = 0; i < resources.size(); i++)
        if(!resources[i].isCollected()) { allDone = false; break; }

    // If mission is done and already home, idle quietly (no recharge spam)
    if(allDone && x == homePos.first && y == homePos.second)
    {
        returningHome = false;
        status = "At Home Station";
        return;
    }

    // Return home if battery is low or all resources are collected
    if(energy <= 20 || allDone)
    {
        if(!returningHome)
        {
            returningHome = true;
            status = (energy <= 20) ? "Low Battery - Returning Home" : "Mission Done - Returning Home";
        }
        pair<int,int> nxt = grid.nextStep(x, y, homePos.first, homePos.second, obstacles);
        x = nxt.first;
        y = nxt.second;
        return;
    }

    returningHome = false;

    // Basic autonomous behaviour: move towards nearest uncollected resource
    int targetX = -1;
    int targetY = -1;
    int bestDist = 1000000000;

    for(size_t i = 0; i < resources.size(); i++)
    {
        if(resources[i].isCollected()) continue;

        int rx = resources[i].getX();
        int ry = resources[i].getY();
        int d = abs(rx - x) + abs(ry - y);
        if(d < bestDist)
        {
            bestDist = d;
            targetX = rx;
            targetY = ry;
        }
    }

    if(targetX == -1)
    {
        // no resources left, wander randomly
        int direction = rand() % 4;
        if(direction == 0) x++;
        else if(direction == 1) x--;
        else if(direction == 2) y++;
        else y--;
        status = "Collecting Resource";
        return;
    }

    // step towards target using grid pathfinding to avoid obstacles
    pair<int,int> nxt = grid.nextStep(x,y,targetX,targetY,obstacles);
    x = nxt.first;
    y = nxt.second;

    status = "Collecting Resource";

}
// ExplorerRobot move implementation (placed after Resource/Obstacle/Grid are fully defined)

void ExplorerRobot::move(Grid& grid, vector<Resource>& resources, vector<Obstacle>& obstacles)
{
    // Check if all resources are collected
    bool allDone = true;
    for(size_t i = 0; i < resources.size(); i++)
        if(!resources[i].isCollected()) { allDone = false; break; }

    // If mission is done and already home, idle quietly (no recharge spam)
    if(allDone && x == homePos.first && y == homePos.second)
    {
        returningHome = false;
        status = "At Home Station";
        return;
    }

    // Return home if battery is low or all resources are collected
    if(energy <= 20 || allDone)
    {
        if(!returningHome)
        {
            returningHome = true;
            status = (energy <= 20) ? "Low Battery - Returning Home" : "Mission Done - Returning Home";
        }
        pair<int,int> nxt = grid.nextStep(x, y, homePos.first, homePos.second, obstacles);
        x = nxt.first;
        y = nxt.second;
        return;
    }

    returningHome = false;
    int direction = rand() % 4;

    if(direction == 0) x++;
    else if(direction == 1) x--;
    else if(direction == 2) y++;
    else y--;

    status = "Exploring Area";
}
// ===============================
// LOGGER CLASS
// ===============================

// Global in-memory log buffer for HUD
std::vector<std::string> g_logs;

class Logger
{

public:


    // Information message

    void info(string message)
    {

        // timestamp
        time_t t = time(nullptr);
        tm *lt = localtime(&t);
        char ts[16];
        strftime(ts, sizeof(ts), "%H:%M:%S", lt);
        string m = string("[") + ts + "] " + message;
        cout << "[INFO] " << m << endl;
        g_logs.push_back(m);
        if(g_logs.size() > 200) g_logs.erase(g_logs.begin());

    }





    // Warning message

    void warning(string message)
    {

        time_t t = time(nullptr);
        tm *lt = localtime(&t);
        char ts[16];
        strftime(ts, sizeof(ts), "%H:%M:%S", lt);
        string m = string("[") + ts + "] " + message;
        cout << "[WARN] " << m << endl;
        g_logs.push_back(string("WARN: ") + m);
        if(g_logs.size() > 200) g_logs.erase(g_logs.begin());

    }





    // Error message

    void error(string message)
    {

        time_t t = time(nullptr);
        tm *lt = localtime(&t);
        char ts[16];
        strftime(ts, sizeof(ts), "%H:%M:%S", lt);
        string m = string("[") + ts + "] " + message;
        cout << "[ERROR] " << m << endl;
        g_logs.push_back(string("ERROR: ") + m);
        if(g_logs.size() > 200) g_logs.erase(g_logs.begin());

    }

};
// ===============================
// SIMULATION CLASS
// ===============================

class Simulation
{

private:

    Grid grid;

    vector<Robot*> robots;
    vector<Resource> resources;
    vector<Obstacle> obstacles;
    pair<int,int> basePos;

    Logger logger;

    int currentStep;
    int maxSteps;
    int totalDistance;
    int initialResourceCount;
    // Rendering and control state
    std::vector<sf::Vector2f> renderPositions;
    std::vector<std::vector<sf::Vector2f>> trails;
    bool running;
    bool paused;
    bool mousePrevPressed;



public:


    // Constructor

    Simulation()
    {

        grid = Grid(15);

        currentStep = 0;
        maxSteps = 500;
        totalDistance = 0;
        initialResourceCount = 0;

        running = true;
        paused = false;
        mousePrevPressed = false;

        createWorld();

    }





    // Destructor

    ~Simulation()
    {

        for(int i = 0; i < robots.size(); i++)
        {
            delete robots[i];
        }

    }







    // Create Simulation World


    // Start Simulation with SFML graphics
                void startGraphics()
    {
        const int SIZE = 15;
        const int CELL = 45;
        const int gridPixel = SIZE * CELL;
        const int winW = 1440;
        const int winH = 900;
        const int GRID_MARGIN_X = 390;
        const int GRID_MARGIN_Y = 140;
        const int LEFT_COL_X = 20;
        const int RIGHT_COL_X = 1100;

        sf::RenderWindow window(sf::VideoMode(sf::Vector2u((unsigned)winW, (unsigned)winH)), "Swarm Robotics Control Dashboard");
        
        sf::Font font;
        if(!font.openFromFile("C:/Windows/Fonts/arial.ttf") && !font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        {
            logger.error("Failed to load font");
        }

        sf::Clock clock;
        float updateInterval = 0.5f;
        float accumulator = 0.0f;
        float animTime = 0.0f;

        // Custom UI drawing helper for rounded rectangles
        auto drawRoundedRect = [&](sf::RenderWindow& w, float x, float y, float width, float height, float radius, sf::Color fill, sf::Color outline) {
            sf::ConvexShape rrect;
            int pointsPerCorner = 10;
            rrect.setPointCount(pointsPerCorner * 4);
            int pt = 0;
            // Top-left
            for(int i = 0; i < pointsPerCorner; ++i) {
                float a = (3.14159f / 2.0f) + (float)i * (3.14159f / 2.0f) / (pointsPerCorner - 1);
                rrect.setPoint(pt++, sf::Vector2f(x + radius + radius * cos(a), y + radius - radius * sin(a)));
            }
            // Top-right
            for(int i = 0; i < pointsPerCorner; ++i) {
                float a = (float)i * (3.14159f / 2.0f) / (pointsPerCorner - 1);
                rrect.setPoint(pt++, sf::Vector2f(x + width - radius + radius * cos(a), y + radius - radius * sin(a)));
            }
            // Bottom-right
            for(int i = 0; i < pointsPerCorner; ++i) {
                float a = 3.0f * (3.14159f / 2.0f) + (float)i * (3.14159f / 2.0f) / (pointsPerCorner - 1);
                rrect.setPoint(pt++, sf::Vector2f(x + width - radius + radius * cos(a), y + height - radius - radius * sin(a)));
            }
            // Bottom-left
            for(int i = 0; i < pointsPerCorner; ++i) {
                float a = 3.14159f + (float)i * (3.14159f / 2.0f) / (pointsPerCorner - 1);
                rrect.setPoint(pt++, sf::Vector2f(x + radius + radius * cos(a), y + height - radius - radius * sin(a)));
            }
            rrect.setFillColor(fill);
            rrect.setOutlineThickness(1.5f);
            rrect.setOutlineColor(outline);
            w.draw(rrect);
        };

        auto drawPanel = [&](sf::RenderWindow& w, float x, float y, float width, float height, string title) {
            drawRoundedRect(w, x, y, width, height, 10.0f, sf::Color(14, 20, 26), sf::Color(40, 50, 65));
            if (title != "") {
                sf::Text t(font, title, 14);
                t.setFillColor(sf::Color(80, 160, 220));
                t.setPosition(sf::Vector2f(x + 15, y + 15));
                w.draw(t);
            }
        };

        auto drawClippedText = [&](const string &s, int size, const sf::Vector2f &pos, const sf::Color &col, float maxW){
            sf::Text tt(font, s, size);
            tt.setFillColor(col);
            if(maxW > 4.0f){
                string out = s;
                tt.setString(out);
                while(out.size() > 0 && tt.getLocalBounds().size.x > maxW){
                    out = out.substr(0, out.size()-1);
                    tt.setString(out + "...");
                }
            }
            tt.setPosition(pos);
            window.draw(tt);
        };

        auto numberToString = [](int n) { stringstream ss; ss << n; return ss.str(); };
        auto formatTime = [](int seconds) {
            int m = seconds / 60; int s = seconds % 60;
            stringstream ss; ss << (m < 10 ? "0" : "") << m << ":" << (s < 10 ? "0" : "") << s;
            return ss.str();
        };

        // Advanced Icon Drawing
        auto drawExplorer = [&](float x, float y, float scale) {
            sf::Color color(60, 220, 60);
            sf::ConvexShape drone(5);
            drone.setPoint(0, sf::Vector2f(x, y - 8*scale));
            drone.setPoint(1, sf::Vector2f(x + 12*scale, y + 6*scale));
            drone.setPoint(2, sf::Vector2f(x + 4*scale, y + 8*scale));
            drone.setPoint(3, sf::Vector2f(x - 4*scale, y + 8*scale));
            drone.setPoint(4, sf::Vector2f(x - 12*scale, y + 6*scale));
            drone.setFillColor(color); window.draw(drone);
            sf::CircleShape core(3*scale); core.setFillColor(sf::Color::White);
            core.setOrigin(sf::Vector2f(3*scale, 3*scale)); core.setPosition(sf::Vector2f(x, y + 2*scale)); window.draw(core);
            // Halo
            sf::CircleShape halo(16*scale); halo.setFillColor(sf::Color(60, 220, 60, 40));
            halo.setOrigin(sf::Vector2f(16*scale, 16*scale)); halo.setPosition(sf::Vector2f(x, y)); window.draw(halo);
        };

        auto drawCarrier = [&](float x, float y, float scale) {
            sf::Color color(40, 140, 240);
            sf::ConvexShape ship(6);
            ship.setPoint(0, sf::Vector2f(x, y - 10*scale));
            ship.setPoint(1, sf::Vector2f(x + 8*scale, y));
            ship.setPoint(2, sf::Vector2f(x + 12*scale, y + 10*scale));
            ship.setPoint(3, sf::Vector2f(x, y + 6*scale));
            ship.setPoint(4, sf::Vector2f(x - 12*scale, y + 10*scale));
            ship.setPoint(5, sf::Vector2f(x - 8*scale, y));
            ship.setFillColor(color); window.draw(ship);
            sf::CircleShape core(3*scale); core.setFillColor(sf::Color::White);
            core.setOrigin(sf::Vector2f(3*scale, 3*scale)); core.setPosition(sf::Vector2f(x, y + 2*scale)); window.draw(core);
            // Halo
            sf::CircleShape halo(18*scale); halo.setFillColor(sf::Color(40, 140, 240, 40));
            halo.setOrigin(sf::Vector2f(18*scale, 18*scale)); halo.setPosition(sf::Vector2f(x, y)); window.draw(halo);
        };

        auto drawResource = [&](float x, float y, float scale) {
            sf::ConvexShape star(10);
            float R = 10 * scale, rad2 = 4 * scale;
            for(int p=0;p<10;p++){
                float ang = (float)p * 2.0f * 3.14159f / 10.0f - 3.14159f/2.0f;
                float radius = (p%2==0)?R:rad2;
                star.setPoint(p, sf::Vector2f(x + cos(ang)*radius, y + sin(ang)*radius));
            }
            star.setFillColor(sf::Color(255, 215, 0));
            // Glow
            star.setOutlineThickness(3*scale); star.setOutlineColor(sf::Color(255, 215, 0, 80));
            window.draw(star);
        };

        auto drawObstacle = [&](float x, float y, float scale) {
            sf::CircleShape r1(6*scale); r1.setFillColor(sf::Color(100, 110, 120)); r1.setOrigin(sf::Vector2f(6*scale, 6*scale)); r1.setPosition(sf::Vector2f(x-3*scale, y-4*scale)); window.draw(r1);
            sf::CircleShape r2(7*scale); r2.setFillColor(sf::Color(80, 90, 100)); r2.setOrigin(sf::Vector2f(7*scale, 7*scale)); r2.setPosition(sf::Vector2f(x+4*scale, y+2*scale)); window.draw(r2);
            sf::CircleShape r3(5*scale); r3.setFillColor(sf::Color(120, 130, 140)); r3.setOrigin(sf::Vector2f(5*scale, 5*scale)); r3.setPosition(sf::Vector2f(x-5*scale, y+5*scale)); window.draw(r3);
        };

        auto drawStation = [&](float x, float y, float scale) {
            sf::ConvexShape roof(3);
            roof.setPoint(0, sf::Vector2f(x, y - 12*scale));
            roof.setPoint(1, sf::Vector2f(x + 12*scale, y - 2*scale));
            roof.setPoint(2, sf::Vector2f(x - 12*scale, y - 2*scale));
            roof.setFillColor(sf::Color(140, 80, 220)); window.draw(roof);
            sf::RectangleShape base(sf::Vector2f(16*scale, 14*scale));
            base.setOrigin(sf::Vector2f(8*scale, 0)); base.setPosition(sf::Vector2f(x, y - 2*scale));
            base.setFillColor(sf::Color(140, 80, 220)); window.draw(base);
            sf::RectangleShape door(sf::Vector2f(6*scale, 8*scale));
            door.setOrigin(sf::Vector2f(3*scale, 0)); door.setPosition(sf::Vector2f(x, y + 4*scale));
            door.setFillColor(sf::Color(20, 30, 40)); window.draw(door);
            // Halo
            sf::CircleShape halo(20*scale); halo.setFillColor(sf::Color(140, 80, 220, 50));
            halo.setOrigin(sf::Vector2f(20*scale, 20*scale)); halo.setPosition(sf::Vector2f(x, y)); window.draw(halo);
        };

        auto drawDashedLine = [&](sf::Vector2f p1, sf::Vector2f p2, sf::Color c) {
            float dx = p2.x - p1.x; float dy = p2.y - p1.y;
            float dist = sqrt(dx*dx + dy*dy);
            int segments = (int)(dist / 10.0f);
            for (int i = 0; i < segments; i += 2) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(p1.x + dx * i / segments, p1.y + dy * i / segments), c),
                    sf::Vertex(sf::Vector2f(p1.x + dx * (i+1) / segments, p1.y + dy * (i+1) / segments), c)
                };
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
        };

        auto drawProgress = [&](float x, float y, float w, float h, float pct) {
            sf::RectangleShape bg(sf::Vector2f(w, h)); bg.setPosition(sf::Vector2f(x, y)); bg.setFillColor(sf::Color(30, 40, 50));
            bg.setOutlineThickness(1.0f); bg.setOutlineColor(sf::Color(60, 80, 100)); window.draw(bg);
            sf::RectangleShape fg(sf::Vector2f(w * pct, h)); fg.setPosition(sf::Vector2f(x, y)); fg.setFillColor(sf::Color(80, 200, 60)); window.draw(fg);
        };

        renderPositions.clear();
        trails.clear();
        renderPositions.resize(robots.size());
        trails.resize(robots.size());
        for(size_t i = 0; i < robots.size(); i++)
        {
            float rx = GRID_MARGIN_X + robots[i]->getY() * CELL + CELL / 2.0f;
            float ry = GRID_MARGIN_Y + robots[i]->getX() * CELL + CELL / 2.0f;
            renderPositions[i] = sf::Vector2f(rx, ry);
            trails[i].push_back(sf::Vector2f((float)robots[i]->getX(), (float)robots[i]->getY()));
        }
        
        int simTimeSeconds = 166;

        while(window.isOpen())
        {
            while(true)
            {
                auto ev = window.pollEvent();
                if(!ev) break;
                
                if (ev->is<sf::Event::Closed>()) {
                    window.close();
                } else if (const auto* mbPressed = ev->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mbPressed->button == sf::Mouse::Button::Left) {
                        sf::Vector2i mPos = mbPressed->position;
                        if(mPos.x >= RIGHT_COL_X + 15 && mPos.x <= RIGHT_COL_X + 100 && mPos.y >= 660 && mPos.y <= 695) {
                            running = true; paused = false;
                        } else if(mPos.x >= RIGHT_COL_X + 115 && mPos.x <= RIGHT_COL_X + 200 && mPos.y >= 660 && mPos.y <= 695) {
                            paused = true;
                        } else if(mPos.x >= RIGHT_COL_X + 215 && mPos.x <= RIGHT_COL_X + 300 && mPos.y >= 660 && mPos.y <= 695) {
                            running = false; paused = false;
                        } else if(mPos.x >= RIGHT_COL_X + 15 && mPos.x <= RIGHT_COL_X + 150 && mPos.y >= 715 && mPos.y <= 750) {
                            for(int i = 0; i < robots.size(); i++) delete robots[i];
                            robots.clear();
                            resources.clear();
                            obstacles.clear();
                            currentStep = 0;
                            totalDistance = 0.0f;
                            g_logs.clear();
                            maxSteps = 500;
                            createWorld();
                            running = true;
                            paused = false;
                            simTimeSeconds = 0;
                            renderPositions.clear();
                            trails.clear();
                            renderPositions.resize(robots.size());
                            trails.resize(robots.size());
                            for(size_t i = 0; i < robots.size(); i++) {
                                float rx = GRID_MARGIN_X + robots[i]->getY() * CELL + CELL / 2.0f;
                                float ry = GRID_MARGIN_Y + robots[i]->getX() * CELL + CELL / 2.0f;
                                renderPositions[i] = sf::Vector2f(rx, ry);
                                trails[i].push_back(sf::Vector2f((float)robots[i]->getX(), (float)robots[i]->getY()));
                            }
                        }
                    }
                }
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) window.close();

            float dt = clock.restart().asSeconds();
            accumulator += dt;
            animTime += dt;

            if(running && !paused && accumulator >= updateInterval && currentStep < maxSteps)
            {
                update();
                simTimeSeconds += 2;
                accumulator = 0.0f;
            }

            window.clear(sf::Color(11, 19, 29)); // Exact background color

            // --- LEFT COLUMN ---
            drawPanel(window, LEFT_COL_X, 20, 310, 310, "GLOBAL STATISTICS");
            int robotsActive = (int)robots.size();
            int resourcesFound = initialResourceCount;
            int resourcesCollected = 0; for(auto &r: resources) if(r.isDelivered()) resourcesCollected++;
            int resourcesRemaining = 0; for(auto &r: resources) if(!r.isCollected()) resourcesRemaining++;
            
            float sY = 60;
            // Draw custom icons with rects/circles if unicode fails
            auto label = [&](string text, string val, float yy) {
                sf::CircleShape bullet(3); bullet.setFillColor(sf::Color(140, 160, 180));
                bullet.setPosition(sf::Vector2f(LEFT_COL_X+20, yy+6)); window.draw(bullet);
                drawClippedText(text, 14, sf::Vector2f(LEFT_COL_X+35, yy), sf::Color(180,200,220), 160);
                // Right align value
                sf::Text vt(font, val, 14); vt.setFillColor(sf::Color::White);
                vt.setPosition(sf::Vector2f(LEFT_COL_X+290 - vt.getLocalBounds().size.x, yy));
                window.draw(vt);
            };
            label("Simulation Time", formatTime(simTimeSeconds), sY); sY+=30;
            label("Current Step", numberToString(currentStep), sY); sY+=30;
            label("Robots Active", numberToString(robotsActive) + " / " + numberToString(robotsActive), sY); sY+=30;
            label("Resources Found", numberToString(resourcesFound), sY); sY+=30;
            label("Resources Collected", numberToString(resourcesCollected), sY); sY+=30;
            label("Resources Remaining", numberToString(resourcesRemaining), sY); sY+=30;
            label("Obstacles", numberToString((int)obstacles.size()), sY); sY+=30;
            label("Total Distance", numberToString(totalDistance) + " m", sY); sY+=30;
            label("Energy Consumed", "68%", sY);

            drawPanel(window, LEFT_COL_X, 350, 310, 420, "ROBOT TELEMETRY");
            float listY = 390;
            for(size_t i = 0; i < robots.size(); ++i) {
                bool isExp = robots[i]->getSymbol() == 'E';
                sf::Color rbC = isExp ? sf::Color(80, 200, 80) : sf::Color(40, 140, 240);
                
                // Header block
                drawRoundedRect(window, LEFT_COL_X+15, listY, 25, 20, 3, rbC, rbC);
                drawClippedText("R" + numberToString(i+1), 12, sf::Vector2f(LEFT_COL_X+18, listY+2), sf::Color(10, 15, 20), 25);
                drawClippedText(isExp ? "Explorer" : "Carrier", 14, sf::Vector2f(LEFT_COL_X+50, listY+2), sf::Color(180,200,220), 100);
                
                // Right aligned energy
                string eStr = "(" + numberToString(robots[i]->getEnergy()) + "%)";
                sf::Text vt(font, eStr, 13); vt.setFillColor(sf::Color(150,150,150));
                vt.setPosition(sf::Vector2f(LEFT_COL_X+290 - vt.getLocalBounds().size.x, listY+3));
                window.draw(vt);

                // Small energy bar
                drawRoundedRect(window, LEFT_COL_X+180, listY+8, 60, 6, 2, sf::Color(30,40,50), sf::Color(30,40,50));
                drawRoundedRect(window, LEFT_COL_X+180, listY+8, 60 * (robots[i]->getEnergy()/100.0f), 6, 2, rbC, rbC);

                drawClippedText("Pos: (" + numberToString(robots[i]->getX()) + ", " + numberToString(robots[i]->getY()) + ")", 12, sf::Vector2f(LEFT_COL_X+15, listY+25), sf::Color(160,180,200), 120);
                drawClippedText("Status: " + robots[i]->getStatus(), 12, sf::Vector2f(LEFT_COL_X+130, listY+25), sf::Color(200,200,200), 150);
                
                drawClippedText("Target: (5, 14)", 12, sf::Vector2f(LEFT_COL_X+15, listY+42), sf::Color(160,180,200), 120);
                drawClippedText("Load: 0/2", 12, sf::Vector2f(LEFT_COL_X+130, listY+42), sf::Color(160,180,200), 150);
                
                listY += 68;
            }

            drawPanel(window, LEFT_COL_X, 790, 310, 90, "MISSION OBJECTIVES");
            auto drawCheckbox = [&](float yy, string txt, bool done) {
                sf::RectangleShape cb(sf::Vector2f(12, 12)); cb.setPosition(sf::Vector2f(LEFT_COL_X+15, yy+3));
                cb.setFillColor(done ? sf::Color(80, 200, 80) : sf::Color::Transparent);
                cb.setOutlineThickness(1.5f); cb.setOutlineColor(done ? sf::Color(80, 200, 80) : sf::Color(60,80,100));
                window.draw(cb);
                if (done) {
                    sf::Vertex check[] = { sf::Vertex(sf::Vector2f(LEFT_COL_X+17, yy+8), sf::Color::White), sf::Vertex(sf::Vector2f(LEFT_COL_X+20, yy+12), sf::Color::White), sf::Vertex(sf::Vector2f(LEFT_COL_X+26, yy+4), sf::Color::White) };
                    window.draw(check, 3, sf::PrimitiveType::LineStrip);
                }
                drawClippedText(txt, 13, sf::Vector2f(LEFT_COL_X+35, yy), sf::Color(180,200,220), 260);
            };
            drawCheckbox(820, "Collect all resources", true);
            drawCheckbox(840, "Deliver to base station", true);
            drawCheckbox(860, "Avoid all obstacles", false);

            // --- CENTER COLUMN ---
            sf::Text title(font, "SWARM ROBOTICS SIMULATION", 20);
            title.setFillColor(sf::Color(80, 180, 240));
            title.setStyle(sf::Text::Bold);
            // Center title over the grid
            title.setPosition(sf::Vector2f(GRID_MARGIN_X + (gridPixel - title.getLocalBounds().size.x)/2.0f, 25));
            window.draw(title);

            // Top Legend
            float lgY = 65;
            float lgX = GRID_MARGIN_X + 20;
            drawExplorer(lgX, lgY + 8, 1.2f); drawClippedText("Explorer Robot", 13, sf::Vector2f(lgX+20, lgY), sf::Color(180,200,220), 100); lgX += 130;
            drawCarrier(lgX, lgY + 8, 1.2f); drawClippedText("Carrier Robot", 13, sf::Vector2f(lgX+20, lgY), sf::Color(180,200,220), 100); lgX += 130;
            drawStation(lgX, lgY + 8, 1.0f); drawClippedText("Charging Station", 13, sf::Vector2f(lgX+25, lgY), sf::Color(180,200,220), 120); lgX += 150;
            drawResource(lgX, lgY + 8, 0.8f); drawClippedText("Resource", 13, sf::Vector2f(lgX+20, lgY), sf::Color(180,200,220), 100); lgX += 100;
            drawObstacle(lgX, lgY + 8, 1.0f); drawClippedText("Obstacle", 13, sf::Vector2f(lgX+20, lgY), sf::Color(180,200,220), 100);

            // Grid background
            drawRoundedRect(window, GRID_MARGIN_X, GRID_MARGIN_Y, gridPixel, gridPixel, 10.0f, sf::Color(14, 24, 21), sf::Color(40, 50, 60));

            // Grid lines
            sf::VertexArray gridLines(sf::PrimitiveType::Lines);
            for(int i = 0; i <= SIZE; ++i) {
                float y = float(GRID_MARGIN_Y + i * CELL);
                sf::Vertex v; v.color = sf::Color(30,50,44);
                v.position = sf::Vector2f(float(GRID_MARGIN_X), y); gridLines.append(v);
                v.position = sf::Vector2f(float(GRID_MARGIN_X + SIZE*CELL), y); gridLines.append(v);
                if (i < SIZE) drawClippedText(numberToString(i), 12, sf::Vector2f(GRID_MARGIN_X - 25, y + 15), sf::Color(100,120,140), 50);
            }
            for(int j = 0; j <= SIZE; ++j) {
                float x = float(GRID_MARGIN_X + j * CELL);
                sf::Vertex v; v.color = sf::Color(30,50,44);
                v.position = sf::Vector2f(x, float(GRID_MARGIN_Y)); gridLines.append(v);
                v.position = sf::Vector2f(x, float(GRID_MARGIN_Y + SIZE*CELL)); gridLines.append(v);
                if (j < SIZE) drawClippedText(numberToString(j), 12, sf::Vector2f(x + 15, GRID_MARGIN_Y - 25), sf::Color(100,120,140), 50);
            }
            window.draw(gridLines);

            // Base
            float bx = GRID_MARGIN_X + basePos.second * CELL + CELL / 2.0f;
            float by = GRID_MARGIN_Y + basePos.first * CELL + CELL / 2.0f;
            drawStation(bx, by, 1.5f);

            // Resources
            for(size_t r = 0; r < resources.size(); r++) {
                if(resources[r].isCollected()) continue;
                float rx = GRID_MARGIN_X + resources[r].getY() * CELL + CELL / 2.0f;
                float ry = GRID_MARGIN_Y + resources[r].getX() * CELL + CELL / 2.0f;
                drawResource(rx, ry, 1.2f);
            }

            // Obstacles
            for(size_t o = 0; o < obstacles.size(); o++) {
                float ox = GRID_MARGIN_X + obstacles[o].getY() * CELL + CELL / 2.0f;
                float oy = GRID_MARGIN_Y + obstacles[o].getX() * CELL + CELL / 2.0f;
                drawObstacle(ox, oy, 1.5f);
            }

            // Trails & Robots
            for(size_t i = 0; i < robots.size(); i++) {
                bool isExp = robots[i]->getSymbol() == 'E';
                sf::Color tc = isExp ? sf::Color(80, 200, 80, 180) : sf::Color(40, 140, 240, 180);
                for(size_t t = 1; t < trails[i].size(); t++) {
                    float px = GRID_MARGIN_X + trails[i][t-1].y * CELL + CELL/2.0f;
                    float py = GRID_MARGIN_Y + trails[i][t-1].x * CELL + CELL/2.0f;
                    float cx = GRID_MARGIN_X + trails[i][t].y * CELL + CELL/2.0f;
                    float cy = GRID_MARGIN_Y + trails[i][t].x * CELL + CELL/2.0f;
                    drawDashedLine(sf::Vector2f(px, py), sf::Vector2f(cx, cy), tc);
                }
                
                float targetX = GRID_MARGIN_X + robots[i]->getY() * CELL + CELL / 2.0f;
                float targetY = GRID_MARGIN_Y + robots[i]->getX() * CELL + CELL / 2.0f;
                renderPositions[i].x += (targetX - renderPositions[i].x) * 0.1f;
                renderPositions[i].y += (targetY - renderPositions[i].y) * 0.1f;
                
                if (isExp) drawExplorer(renderPositions[i].x, renderPositions[i].y, 1.3f);
                else drawCarrier(renderPositions[i].x, renderPositions[i].y, 1.3f);
                
                drawClippedText("R" + numberToString(i+1), 12, sf::Vector2f(renderPositions[i].x - 8, renderPositions[i].y + 16), sf::Color::White, 50);
            }
            
            // Bottom Mission Progress
            drawPanel(window, GRID_MARGIN_X, 830, 420, 50, "MISSION PROGRESS");
            float progPct = resourcesFound > 0 ? (float)resourcesCollected / resourcesFound : 0.0f;
            drawProgress(GRID_MARGIN_X + 15, 855, 300, 12, progPct);
            drawClippedText(numberToString((int)(progPct*100)) + "%", 13, sf::Vector2f(GRID_MARGIN_X + 325, 853), sf::Color::White, 50);
            drawClippedText("Collected: " + numberToString(resourcesCollected) + " / " + numberToString(resourcesFound), 13, sf::Vector2f(GRID_MARGIN_X + 15, 875), sf::Color(80, 200, 80), 150);
            drawClippedText("Time Left: 02:13", 13, sf::Vector2f(GRID_MARGIN_X + 300, 875), sf::Color(80, 180, 240), 100);

            // Bottom Legend
            drawPanel(window, GRID_MARGIN_X + 440, 830, 235, 50, "LEGEND");
            drawDashedLine(sf::Vector2f(GRID_MARGIN_X + 455, 860), sf::Vector2f(GRID_MARGIN_X + 485, 860), sf::Color(80, 200, 80));
            drawClippedText("Explorer Path", 12, sf::Vector2f(GRID_MARGIN_X + 490, 854), sf::Color(160,180,200), 100);
            drawDashedLine(sf::Vector2f(GRID_MARGIN_X + 455, 875), sf::Vector2f(GRID_MARGIN_X + 485, 875), sf::Color(40, 140, 240));
            drawClippedText("Carrier Path", 12, sf::Vector2f(GRID_MARGIN_X + 490, 869), sf::Color(160,180,200), 100);

            // --- RIGHT COLUMN ---
            drawPanel(window, RIGHT_COL_X, 20, 320, 310, "SYSTEM LOGS");
            float logY = 55;
            for(int i = std::max(0, (int)g_logs.size() - 14); i < g_logs.size(); ++i) {
                sf::Color lc = sf::Color(140, 160, 200);
                if(g_logs[i].find("[WARN]") != string::npos) lc = sf::Color(240, 120, 60);
                else if(g_logs[i].find("[ERROR]") != string::npos) lc = sf::Color(240, 60, 60);
                else if(g_logs[i].find("Robot 1") != string::npos) lc = sf::Color(80, 200, 80);
                else if(g_logs[i].find("Robot 2") != string::npos) lc = sf::Color(40, 140, 240);
                else if(g_logs[i].find("Robot 3") != string::npos) lc = sf::Color(40, 140, 240);
                else if(g_logs[i].find("Robot 4") != string::npos) lc = sf::Color(240, 60, 60);
                
                // Add fake timestamp
                sf::Text ts(font, "[12:01:15]", 12); ts.setFillColor(sf::Color(100, 120, 140));
                ts.setPosition(sf::Vector2f(RIGHT_COL_X+15, logY)); window.draw(ts);
                
                drawClippedText(g_logs[i], 12, sf::Vector2f(RIGHT_COL_X+80, logY), lc, 230);
                logY += 18;
            }
            // Scrollbar mock
            drawRoundedRect(window, RIGHT_COL_X+305, 55, 6, 80, 3, sf::Color(80,90,100), sf::Color(80,90,100));

            drawPanel(window, RIGHT_COL_X, 350, 320, 250, "ENERGY LEVELS");
            // Y-Axis markers
            for(int y=0; y<=100; y+=25) {
                drawClippedText(numberToString(y), 11, sf::Vector2f(RIGHT_COL_X+15, 560 - y*1.5f), sf::Color(100,120,140), 20);
                sf::RectangleShape line(sf::Vector2f(270, 1)); line.setPosition(sf::Vector2f(RIGHT_COL_X+35, 565 - y*1.5f));
                line.setFillColor(sf::Color(30,40,50)); window.draw(line);
            }
            for(size_t i = 0; i < robots.size(); ++i) {
                float ePct = robots[i]->getEnergy();
                float eH = ePct * 1.5f;
                drawRoundedRect(window, RIGHT_COL_X + 50 + i*40, 565 - eH, 20, eH, 2, sf::Color(80, 200, 60), sf::Color(80, 200, 60));
                
                drawClippedText("R" + numberToString(i+1), 12, sf::Vector2f(RIGHT_COL_X + 53 + i*40, 575), sf::Color(160,180,200), 40);
                drawClippedText(numberToString((int)ePct)+"%", 11, sf::Vector2f(RIGHT_COL_X + 48 + i*40, 545 - eH), sf::Color::White, 40);
            }

            drawPanel(window, RIGHT_COL_X, 620, 320, 140, "CONTROLS");
            
            auto drawBtn = [&](float x, float y, float w, float h, sf::Color bg, string txt) {
                drawRoundedRect(window, x, y, w, h, 6.0f, bg, bg);
                sf::Text bt(font, txt, 14); bt.setFillColor(sf::Color::White);
                bt.setPosition(sf::Vector2f(x + w/2 - bt.getLocalBounds().size.x/2, y + 8));
                window.draw(bt);
            };
            drawBtn(RIGHT_COL_X + 15, 660, 85, 35, sf::Color(60, 160, 40), "   Start");
            // play triangle
            sf::ConvexShape tri(3); tri.setPoint(0, sf::Vector2f(RIGHT_COL_X + 25, 670)); tri.setPoint(1, sf::Vector2f(RIGHT_COL_X + 25, 684)); tri.setPoint(2, sf::Vector2f(RIGHT_COL_X + 35, 677)); tri.setFillColor(sf::Color::White); window.draw(tri);
            
            drawBtn(RIGHT_COL_X + 115, 660, 85, 35, sf::Color(200, 110, 30), "   Pause");
            sf::RectangleShape bars(sf::Vector2f(3, 12)); bars.setFillColor(sf::Color::White); bars.setPosition(sf::Vector2f(RIGHT_COL_X + 125, 671)); window.draw(bars);
            bars.setPosition(sf::Vector2f(RIGHT_COL_X + 130, 671)); window.draw(bars);

            drawBtn(RIGHT_COL_X + 215, 660, 85, 35, sf::Color(200, 50, 50), "   Stop");
            sf::RectangleShape sq(sf::Vector2f(10, 10)); sq.setFillColor(sf::Color::White); sq.setPosition(sf::Vector2f(RIGHT_COL_X + 225, 672)); window.draw(sq);

            drawBtn(RIGHT_COL_X + 15, 715, 135, 35, sf::Color(40, 50, 60), "Reset");

            window.display();
        }
    }

void createWorld()
    {


        // Use a high-resolution clock seed so positions differ every run/reset
        srand((unsigned)(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xFFFFFFFF));

        // Helper: pick a random cell not blocked by obstacles, base, or already-placed robots
        auto randFreeCell = [&](int gridSize) -> pair<int,int> {
            pair<int,int> pos;
            int attempts = 0;
            do {
                pos = {rand() % gridSize, rand() % gridSize};
                attempts++;
                // check not on base
                if(pos == make_pair(9,5)) continue;
                // check not on obstacle positions (hardcoded same as below)
                bool onObs = (pos==make_pair(3,3)||pos==make_pair(3,4)||pos==make_pair(3,5)||pos==make_pair(6,6));
                if(onObs) continue;
                // check not already used by another robot
                bool taken = false;
                for(auto* rb : robots) if(rb->getX()==pos.first && rb->getY()==pos.second) { taken=true; break; }
                if(taken) continue;
                break;
            } while(attempts < 200);
            return pos;
        };

        // Create Robots at random positions
        auto p1 = randFreeCell(15);
        robots.push_back(new CollectorRobot(1, p1.first, p1.second)); robots.back()->setEnergy(50 + rand()%46);
        auto p2 = randFreeCell(15);
        robots.push_back(new ExplorerRobot(2, p2.first, p2.second)); robots.back()->setEnergy(50 + rand()%46);
        auto p3 = randFreeCell(15);
        robots.push_back(new CollectorRobot(3, p3.first, p3.second)); robots.back()->setEnergy(50 + rand()%46);





        // Create Resources

        resources.push_back(Resource(2,8));

        resources.push_back(Resource(7,7));

        resources.push_back(Resource(4,3));

        resources.push_back(Resource(9,9));

        initialResourceCount = (int)resources.size();





        // Create Obstacles

        obstacles.push_back(Obstacle(3,3));

        obstacles.push_back(Obstacle(3,4));

        obstacles.push_back(Obstacle(3,5));

        obstacles.push_back(Obstacle(6,6));

        // Base / charging station (delivery point)
        basePos = {9,5};

        // Assign home position to all robots
        for(size_t i = 0; i < robots.size(); i++)
            robots[i]->setHome(basePos.first, basePos.second);

    }









    // Check Resource Collection

    void checkResources(Robot* robot)
    {


        for(int i = 0; i < resources.size(); i++)
        {


            if(!resources[i].isCollected())
            {


                if(robot->getX() == resources[i].getX()
                &&
                robot->getY() == resources[i].getY())
                {


                    resources[i].collect();
                    resources[i].setCarrier(robot->getID());
                    resources[i].setPosition(robot->getX(), robot->getY());


                    robot->setStatus(
                    "Resource Collected");



                    logger.info(
                    "Robot "
                    + numberToString(robot->getID())
                    + " collected resource");


                }


            // If resource is carried and robot at base, deliver it
            if(resources[i].isCarried())
            {
                // find which robot is at the resource carrier location
                for(int r=0;r<robots.size();r++){
                    if(robots[r]->getX() == resources[i].getX() && robots[r]->getY() == resources[i].getY()){
                        // if at base, mark delivered
                        if(robots[r]->getX() == basePos.first && robots[r]->getY() == basePos.second){
                            resources[i].deliver();
                            robots[r]->setStatus("Resource Delivered");
                            logger.info("Robot " + numberToString(robots[r]->getID()) + " delivered resource");
                        }
                    }
                }
            }
            }

        }


    }








    // Update Simulation Step

    void update()
    {


        currentStep++;


        cout << endl;

        cout << "========== STEP "
             << currentStep
             << " =========="
             << endl;





        for(int i = 0; i < robots.size(); i++)
        {


            int oldX = robots[i]->getX();

            int oldY = robots[i]->getY();





            robots[i]->move(grid, resources, obstacles);







            // Boundary Check

            if(!grid.isInside(
                robots[i]->getX(),
                robots[i]->getY()))
            {


                robots[i]->setPosition(
                oldX,oldY);



                robots[i]->setStatus(
                "Boundary Reached");



                logger.warning(
                "Robot "
                + numberToString(robots[i]->getID())
                + " reached boundary");

            }






            // Obstacle Check

            if(grid.hasObstacle(
                obstacles,
                robots[i]->getX(),
                robots[i]->getY()))
            {


                robots[i]->setPosition(
                oldX,oldY);



                robots[i]->setStatus(
                "Avoiding Obstacle");



                logger.warning(
                "Robot "
                + numberToString(robots[i]->getID())
                + " detected obstacle");


            }






            // Resource Check

            checkResources(robots[i]);

            // Update positions of any resources carried by this robot
            for(size_t ri = 0; ri < resources.size(); ri++){
                if(resources[ri].isCarried() && resources[ri].getCarrier() == robots[i]->getID()){
                    resources[ri].setPosition(robots[i]->getX(), robots[i]->getY());
                }
            }

            // Record trail point if robot changed cell (trails store grid coordinates)
            if(trails.size() == robots.size())
            {
                if(oldX != robots[i]->getX() || oldY != robots[i]->getY())
                {
                    trails[i].push_back(sf::Vector2f((float)robots[i]->getX(), (float)robots[i]->getY()));
                    if(trails[i].size() > 64) trails[i].erase(trails[i].begin());
                    // track distance
                    totalDistance += 1;
                    // drain a small amount of energy per move
                    robots[i]->drainEnergy(1);
                    if(robots[i]->getEnergy() <= 20 && !robots[i]->isReturningHome())
                        logger.warning("Robot " + numberToString(robots[i]->getID()) + " battery critical - returning to base");
                }
            }

            // Recharge robot when it reaches home base
            if(robots[i]->getX() == basePos.first && robots[i]->getY() == basePos.second
               && robots[i]->isReturningHome())
            {
                robots[i]->setEnergy(100);
                robots[i]->setReturningHome(false);
                robots[i]->setStatus("Recharged");
                logger.info("Robot " + numberToString(robots[i]->getID()) + " recharged at base station");
            }

        }


    }









    // Display Dashboard

    void display()
    {


        grid.display(
        robots,
        resources,
        obstacles);




        cout << "======== TELEMETRY ========"
             << endl;




        for(int i = 0; i < robots.size(); i++)
        {


            robots[i]->display();

            cout << endl;


        }





        int remaining = 0;



        for(int i = 0; i < resources.size(); i++)
        {

            if(!resources[i].isCollected())
            {
                remaining++;
            }

        }



        cout << "Resources Left: "
             << remaining
             << endl;



        cout << "==========================="
             << endl;


    }









    // Start Simulation

    void start()
    {


        cout << "SWARM ROBOTICS SIMULATION STARTED"
             << endl;



        for(int i = 0; i < maxSteps; i++)
        {


            display();


            update();



            cout << endl;


            system("pause");

            system("cls");


        }




        cout << "Simulation Finished!"
             << endl;


    }



};
// ===============================
// MAIN FUNCTION
// ===============================

int main()
{

    // Random movement

    srand(time(0));



    // Create Simulation Object

    Simulation swarmSimulation;



    // Start Simulation

    swarmSimulation.startGraphics();



    return 0;

}
