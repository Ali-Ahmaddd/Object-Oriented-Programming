void createWorld()
    {


        // Create Robots
        srand((unsigned)time(nullptr));
        robots.push_back(new CollectorRobot(1,1,1)); robots.back()->setEnergy(50 + rand()%46);
        robots.push_back(new ExplorerRobot(2,5,5)); robots.back()->setEnergy(50 + rand()%46);
        robots.push_back(new CollectorRobot(3,8,2)); robots.back()->setEnergy(50 + rand()%46);





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
                    if(robots[i]->getEnergy() <= 20) robots[i]->setStatus("Low battery");
                }
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