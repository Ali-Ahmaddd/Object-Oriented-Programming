import sys

cpp_code = r'''    void startGraphics()
    {
        const int SIZE = 15;
        const int CELL = 48;
        const int gridPixel = SIZE * CELL;
        const int winW = 1440;
        const int winH = 900;
        const int GRID_MARGIN_X = 360;
        const int GRID_MARGIN_Y = 100;
        const int LEFT_COL_X = 20;
        const int RIGHT_COL_X = 1110;

        sf::RenderWindow window(sf::VideoMode(sf::Vector2u((unsigned)winW, (unsigned)winH)), "Swarm Robotics Control Dashboard");
        
        // Textures: try to load sprite files; if missing, generate procedural textures at runtime
        sf::Texture texExplorer, texCollector, texResource, texObstacle, texBase;
        bool useTextures = false;

        // Try to load sprite files from working dir or assets/; fall back to shapes if any missing
        bool loadedExplorer = texExplorer.loadFromFile("assets/explorer.png") || texExplorer.loadFromFile("explorer.png");
        bool loadedCollector = texCollector.loadFromFile("assets/collector.png") || texCollector.loadFromFile("collector.png");
        bool loadedResource = texResource.loadFromFile("assets/resource.png") || texResource.loadFromFile("resource.png");
        bool loadedObstacle = texObstacle.loadFromFile("assets/obstacle.png") || texObstacle.loadFromFile("obstacle.png");
        bool loadedBase = texBase.loadFromFile("assets/base.png") || texBase.loadFromFile("base.png");
        useTextures = (loadedExplorer && loadedCollector && loadedResource && loadedObstacle && loadedBase);

        sf::Font font;
        if(!font.openFromFile("C:/Windows/Fonts/arial.ttf") && !font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        {
            logger.error("Failed to load font");
        }

        sf::Clock clock;
        float updateInterval = 0.5f;
        float accumulator = 0.0f;
        float animTime = 0.0f;

        // Custom UI drawing helper
        auto drawPanel = [&](sf::RenderWindow& w, float x, float y, float width, float height, string title) {
            sf::RectangleShape rect(sf::Vector2f(width, height));
            rect.setPosition(sf::Vector2f(x, y));
            rect.setFillColor(sf::Color(18, 24, 31));
            rect.setOutlineThickness(1.5f);
            rect.setOutlineColor(sf::Color(40, 50, 65));
            w.draw(rect);
            
            if (title != "") {
                sf::Text t(font, title, 14);
                t.setFillColor(sf::Color(80, 160, 220));
                t.setPosition(sf::Vector2f(x + 15, y + 15));
                w.draw(t);
            }
        };

        // Helper: draw text clipped to a maximum width
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

        auto numberToString = [](int n) {
            stringstream ss; ss << n; return ss.str();
        };

        auto drawRobotFallback = [&](sf::RenderWindow& window, const sf::Vector2f& pos, bool collector, float size) {
            sf::Color bodyColor = collector ? sf::Color(80, 200, 120) : sf::Color(90, 140, 220);
            sf::Color accent = collector ? sf::Color(255, 220, 90) : sf::Color(140, 200, 255);
            sf::Color dark = sf::Color(18, 22, 30);
            float bodyW = size * 0.56f;
            float bodyH = size * 0.44f;
            float headW = size * 0.34f;
            float headH = size * 0.22f;

            sf::CircleShape halo(bodyW * 0.6f);
            int haloA = 80 + int(40.0f * (std::sin(animTime * 3.0f) + 1.0f) / 2.0f);
            halo.setFillColor(sf::Color(bodyColor.r, bodyColor.g, bodyColor.b, haloA));
            halo.setOrigin(sf::Vector2f(halo.getRadius(), halo.getRadius()));
            halo.setPosition(sf::Vector2f(pos.x, pos.y));
            window.draw(halo);

            sf::RectangleShape body(sf::Vector2f(bodyW, bodyH));
            body.setFillColor(bodyColor);
            body.setOutlineThickness(2.0f);
            body.setOutlineColor(dark);
            body.setOrigin(sf::Vector2f(bodyW/2.0f, bodyH/2.0f));
            body.setPosition(sf::Vector2f(pos.x, pos.y));
            window.draw(body);

            sf::RectangleShape chest(sf::Vector2f(bodyW * 0.56f, bodyH * 0.46f));
            chest.setFillColor(sf::Color(240,245,250));
            chest.setOutlineThickness(1.2f);
            chest.setOutlineColor(dark);
            chest.setOrigin(sf::Vector2f(chest.getSize().x/2.0f, chest.getSize().y/2.0f));
            chest.setPosition(sf::Vector2f(pos.x, pos.y + bodyH*0.02f));
            window.draw(chest);

            sf::RectangleShape head(sf::Vector2f(headW, headH));
            head.setFillColor(sf::Color(230,235,240));
            head.setOutlineThickness(1.8f);
            head.setOutlineColor(dark);
            head.setOrigin(sf::Vector2f(headW/2.0f, headH/2.0f));
            head.setPosition(sf::Vector2f(pos.x, pos.y - bodyH*0.62f));
            window.draw(head);

            sf::RectangleShape visor(sf::Vector2f(headW * 0.66f, headH * 0.44f));
            visor.setFillColor(sf::Color(40,80,140));
            visor.setOrigin(sf::Vector2f(visor.getSize().x/2.0f, visor.getSize().y/2.0f));
            visor.setPosition(sf::Vector2f(pos.x, pos.y - bodyH*0.62f + 2.0f));
            window.draw(visor);

            sf::RectangleShape antenna(sf::Vector2f(3.0f, 8.0f));
            antenna.setFillColor(accent);
            antenna.setOrigin(sf::Vector2f(1.5f, 0.0f));
            antenna.setPosition(sf::Vector2f(pos.x, pos.y - bodyH*0.62f - headH*0.6f - 6.0f));
            window.draw(antenna);
            sf::CircleShape antTip(2.6f);
            antTip.setFillColor(accent);
            antTip.setOrigin(sf::Vector2f(antTip.getRadius(), antTip.getRadius()));
            antTip.setPosition(sf::Vector2f(pos.x, pos.y - bodyH*0.62f - headH*0.6f - 10.0f));
            window.draw(antTip);

            sf::RectangleShape leftArm(sf::Vector2f(6.0f, 18.0f));
            leftArm.setFillColor(dark);
            leftArm.setOrigin(sf::Vector2f(3.0f, 0.0f));
            leftArm.setPosition(sf::Vector2f(pos.x - bodyW*0.52f, pos.y - bodyH*0.15f));
            window.draw(leftArm);

            sf::RectangleShape rightArm = leftArm;
            rightArm.setOrigin(sf::Vector2f(3.0f, 0.0f));
            rightArm.setPosition(sf::Vector2f(pos.x + bodyW*0.52f, pos.y - bodyH*0.15f));
            window.draw(rightArm);

            sf::RectangleShape lLeg(sf::Vector2f(7.0f, 14.0f));
            lLeg.setFillColor(dark);
            lLeg.setOrigin(sf::Vector2f(3.5f, 0.0f));
            lLeg.setPosition(sf::Vector2f(pos.x - bodyW*0.18f, pos.y + bodyH*0.46f));
            window.draw(lLeg);

            sf::RectangleShape rLeg = lLeg;
            rLeg.setPosition(sf::Vector2f(pos.x + bodyW*0.18f, pos.y + bodyH*0.46f));
            window.draw(rLeg);

            if(collector) {
                sf::ConvexShape crate;
                crate.setPointCount(6);
                float w = bodyW * 0.34f, h = bodyH * 0.26f;
                crate.setPoint(0, sf::Vector2f(pos.x - w/2.0f, pos.y - h/2.0f));
                crate.setPoint(1, sf::Vector2f(pos.x + w/2.0f, pos.y - h/2.0f));
                crate.setPoint(2, sf::Vector2f(pos.x + w/2.0f, pos.y + h/2.0f));
                crate.setPoint(3, sf::Vector2f(pos.x, pos.y + h*0.9f));
                crate.setPoint(4, sf::Vector2f(pos.x - w/2.0f, pos.y + h/2.0f));
                crate.setPoint(5, sf::Vector2f(pos.x - w/2.0f, pos.y - h/2.0f));
                crate.setFillColor(sf::Color(220,180,90));
                crate.setOutlineColor(dark);
                crate.setOutlineThickness(1.2f);
                window.draw(crate);
            } else {
                sf::CircleShape core(6.0f);
                core.setFillColor(sf::Color(240,250,255));
                core.setOrigin(sf::Vector2f(core.getRadius(), core.getRadius()));
                core.setPosition(sf::Vector2f(pos.x, pos.y - bodyH*0.08f));
                window.draw(core);
            }
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

        while(window.isOpen())
        {
            while(true)
            {
                auto ev = window.pollEvent();
                if(!ev) break;
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) window.close();

            float dt = clock.restart().asSeconds();
            accumulator += dt;
            animTime += dt;

            if(running && !paused && accumulator >= updateInterval && currentStep < maxSteps)
            {
                update();
                accumulator = 0.0f;
            }

            window.clear(sf::Color(10, 15, 20));

            // --- LEFT COLUMN ---
            drawPanel(window, LEFT_COL_X, 20, 310, 240, "GLOBAL STATISTICS");
            int robotsActive = (int)robots.size();
            int resourcesFound = initialResourceCount;
            int resourcesCollected = 0; for(auto &r: resources) if(r.isDelivered()) resourcesCollected++;
            int resourcesRemaining = 0; for(auto &r: resources) if(!r.isCollected()) resourcesRemaining++;
            
            float sY = 60;
            drawClippedText("Simulation Time", 14, sf::Vector2f(LEFT_COL_X+15, sY), sf::Color(180,200,220), 200);
            drawClippedText("00:02:46", 14, sf::Vector2f(LEFT_COL_X+200, sY), sf::Color::White, 200); sY+=25;
            
            drawClippedText("Current Step", 14, sf::Vector2f(LEFT_COL_X+15, sY), sf::Color(180,200,220), 200);
            drawClippedText(numberToString(currentStep), 14, sf::Vector2f(LEFT_COL_X+200, sY), sf::Color::White, 200); sY+=25;
            
            drawClippedText("Robots Active", 14, sf::Vector2f(LEFT_COL_X+15, sY), sf::Color(180,200,220), 200);
            drawClippedText(numberToString(robotsActive) + " / " + numberToString(robotsActive), 14, sf::Vector2f(LEFT_COL_X+200, sY), sf::Color::White, 200); sY+=25;
            
            drawClippedText("Resources Found", 14, sf::Vector2f(LEFT_COL_X+15, sY), sf::Color(180,200,220), 200);
            drawClippedText(numberToString(resourcesFound), 14, sf::Vector2f(LEFT_COL_X+200, sY), sf::Color::White, 200); sY+=25;
            
            drawClippedText("Total Distance", 14, sf::Vector2f(LEFT_COL_X+15, sY), sf::Color(180,200,220), 200);
            drawClippedText(numberToString(totalDistance) + " m", 14, sf::Vector2f(LEFT_COL_X+200, sY), sf::Color::White, 200); sY+=25;
            
            drawClippedText("Energy Consumed", 14, sf::Vector2f(LEFT_COL_X+15, sY), sf::Color(180,200,220), 200);
            drawClippedText("68%", 14, sf::Vector2f(LEFT_COL_X+200, sY), sf::Color::White, 200);

            drawPanel(window, LEFT_COL_X, 280, 310, 480, "ROBOT TELEMETRY");
            float listY = 320;
            for(size_t i = 0; i < robots.size(); ++i) {
                sf::RectangleShape bg(sf::Vector2f(280, 65));
                bg.setPosition(sf::Vector2f(LEFT_COL_X + 15, listY));
                bg.setFillColor(sf::Color(25, 35, 45));
                window.draw(bg);
                
                sf::Color rbC = robots[i]->getSymbol() == 'E' ? sf::Color(80, 200, 80) : sf::Color(60, 140, 240);
                drawClippedText("R" + numberToString(i+1) + " " + robots[i]->getType(), 13, sf::Vector2f(LEFT_COL_X+20, listY+5), rbC, 200);
                drawClippedText("(" + numberToString(robots[i]->getEnergy()) + "%)", 13, sf::Vector2f(LEFT_COL_X+250, listY+5), sf::Color(150,150,150), 200);
                drawClippedText("Pos: (" + numberToString(robots[i]->getX()) + ", " + numberToString(robots[i]->getY()) + ")", 12, sf::Vector2f(LEFT_COL_X+20, listY+25), sf::Color(180,200,220), 200);
                drawClippedText("Status: " + robots[i]->getStatus(), 12, sf::Vector2f(LEFT_COL_X+130, listY+25), sf::Color(200,200,200), 200);
                
                sf::RectangleShape eBarBg(sf::Vector2f(270, 6));
                eBarBg.setPosition(sf::Vector2f(LEFT_COL_X + 20, listY + 50));
                eBarBg.setFillColor(sf::Color(40,50,60));
                window.draw(eBarBg);
                
                sf::RectangleShape eBar(sf::Vector2f(270 * (robots[i]->getEnergy() / 100.0f), 6));
                eBar.setPosition(sf::Vector2f(LEFT_COL_X + 20, listY + 50));
                eBar.setFillColor(rbC);
                window.draw(eBar);
                
                listY += 75;
            }

            drawPanel(window, LEFT_COL_X, 780, 310, 100, "MISSION OBJECTIVES");
            drawClippedText("[x] Collect all resources", 13, sf::Vector2f(LEFT_COL_X+15, 815), sf::Color(100,220,100), 200);
            drawClippedText("[x] Deliver to base station", 13, sf::Vector2f(LEFT_COL_X+15, 835), sf::Color(100,220,100), 200);
            drawClippedText("[x] Avoid all obstacles", 13, sf::Vector2f(LEFT_COL_X+15, 855), sf::Color(100,220,100), 200);

            // --- CENTER COLUMN ---
            sf::Text title(font, "SWARM ROBOTICS SIMULATION", 20);
            title.setFillColor(sf::Color(80, 180, 240));
            title.setPosition(sf::Vector2f(GRID_MARGIN_X + 220, 20));
            window.draw(title);

            // Grid background
            sf::VertexArray gridBg(sf::PrimitiveType::Triangles);
            for(int i = 0; i < SIZE; ++i) {
                for(int j = 0; j < SIZE; ++j) {
                    float x = float(GRID_MARGIN_X + j * CELL);
                    float y = float(GRID_MARGIN_Y + i * CELL);
                    float s = float(CELL - 2);
                    sf::Vertex v; v.color = sf::Color(18,24,31);
                    v.position = sf::Vector2f(x, y); gridBg.append(v);
                    v.position = sf::Vector2f(x+s, y); gridBg.append(v);
                    v.position = sf::Vector2f(x+s, y+s); gridBg.append(v);
                    v.position = sf::Vector2f(x, y); gridBg.append(v);
                    v.position = sf::Vector2f(x+s, y+s); gridBg.append(v);
                    v.position = sf::Vector2f(x, y+s); gridBg.append(v);
                }
            }
            window.draw(gridBg);

            // Grid lines
            sf::VertexArray gridLines(sf::PrimitiveType::Lines);
            for(int i = 0; i <= SIZE; ++i) {
                float y = float(GRID_MARGIN_Y + i * CELL) - 1.0f;
                sf::Vertex v; v.color = sf::Color(24,40,34);
                v.position = sf::Vector2f(float(GRID_MARGIN_X), y); gridLines.append(v);
                v.position = sf::Vector2f(float(GRID_MARGIN_X + SIZE*CELL), y); gridLines.append(v);
                
                if (i < SIZE) drawClippedText(numberToString(i), 11, sf::Vector2f(GRID_MARGIN_X - 25, y + 20), sf::Color(80,100,120), 50);
            }
            for(int j = 0; j <= SIZE; ++j) {
                float x = float(GRID_MARGIN_X + j * CELL) - 1.0f;
                sf::Vertex v; v.color = sf::Color(24,40,34);
                v.position = sf::Vector2f(x, float(GRID_MARGIN_Y)); gridLines.append(v);
                v.position = sf::Vector2f(x, float(GRID_MARGIN_Y + SIZE*CELL)); gridLines.append(v);
                
                if (j < SIZE) drawClippedText(numberToString(j), 11, sf::Vector2f(x + 20, GRID_MARGIN_Y - 25), sf::Color(80,100,120), 50);
            }
            window.draw(gridLines);

            // Base
            float bx = GRID_MARGIN_X + basePos.second * CELL + CELL / 2.0f;
            float by = GRID_MARGIN_Y + basePos.first * CELL + CELL / 2.0f;
            sf::RectangleShape baseShape(sf::Vector2f(CELL * 0.9f, CELL * 0.9f));
            baseShape.setOrigin(sf::Vector2f(baseShape.getSize().x/2.0f, baseShape.getSize().y/2.0f));
            baseShape.setPosition(sf::Vector2f(bx, by));
            baseShape.setFillColor(sf::Color(140,80,200));
            window.draw(baseShape);

            // Resources
            for(size_t r = 0; r < resources.size(); r++) {
                if(resources[r].isCollected()) continue;
                float rx = GRID_MARGIN_X + resources[r].getY() * CELL + CELL / 2.0f;
                float ry = GRID_MARGIN_Y + resources[r].getX() * CELL + CELL / 2.0f;
                sf::ConvexShape star; star.setPointCount(10);
                float R = CELL * 0.28f, rad2 = R * 0.45f;
                for(int p=0;p<10;p++){
                    float ang = (float)p * 2.0f * 3.14159265f / 10.0f - 3.14159265f/2.0f;
                    float radius = (p%2==0)?R:rad2;
                    star.setPoint(p, sf::Vector2f(rx + cos(ang)*radius, ry + sin(ang)*radius));
                }
                star.setFillColor(sf::Color(255,215,0));
                window.draw(star);
            }

            // Obstacles
            for(size_t o = 0; o < obstacles.size(); o++) {
                float ox = GRID_MARGIN_X + obstacles[o].getY() * CELL + CELL / 2.0f;
                float oy = GRID_MARGIN_Y + obstacles[o].getX() * CELL + CELL / 2.0f;
                sf::CircleShape obs1(CELL*0.25f); obs1.setFillColor(sf::Color(90,95,100));
                obs1.setOrigin(sf::Vector2f(obs1.getRadius(), obs1.getRadius()));
                obs1.setPosition(sf::Vector2f(ox - CELL*0.1f, oy - CELL*0.1f));
                window.draw(obs1);
                sf::CircleShape obs2(CELL*0.3f); obs2.setFillColor(sf::Color(100,110,115));
                obs2.setOrigin(sf::Vector2f(obs2.getRadius(), obs2.getRadius()));
                obs2.setPosition(sf::Vector2f(ox + CELL*0.15f, oy + CELL*0.15f));
                window.draw(obs2);
            }

            // Trails & Robots
            for(size_t i = 0; i < robots.size(); i++) {
                sf::Color tc = robots[i]->getSymbol() == 'E' ? sf::Color(80, 200, 80, 150) : sf::Color(60, 140, 240, 150);
                for(size_t t = 1; t < trails[i].size(); t++) {
                    float px = GRID_MARGIN_X + trails[i][t-1].y * CELL + CELL/2.0f;
                    float py = GRID_MARGIN_Y + trails[i][t-1].x * CELL + CELL/2.0f;
                    float cx = GRID_MARGIN_X + trails[i][t].y * CELL + CELL/2.0f;
                    float cy = GRID_MARGIN_Y + trails[i][t].x * CELL + CELL/2.0f;
                    sf::Vertex line[] = { sf::Vertex(sf::Vector2f(px, py), tc), sf::Vertex(sf::Vector2f(cx, cy), tc) };
                    window.draw(line, 2, sf::PrimitiveType::Lines);
                    
                    sf::CircleShape dot(3);
                    dot.setFillColor(tc);
                    dot.setOrigin(sf::Vector2f(3,3));
                    dot.setPosition(sf::Vector2f(px, py));
                    window.draw(dot);
                }
                
                float targetX = GRID_MARGIN_X + robots[i]->getY() * CELL + CELL / 2.0f;
                float targetY = GRID_MARGIN_Y + robots[i]->getX() * CELL + CELL / 2.0f;
                renderPositions[i].x += (targetX - renderPositions[i].x) * 0.1f;
                renderPositions[i].y += (targetY - renderPositions[i].y) * 0.1f;
                
                drawRobotFallback(window, renderPositions[i], robots[i]->getSymbol() != 'E', CELL*0.8f);
            }
            
            drawPanel(window, GRID_MARGIN_X, 840, 420, 40, "MISSION PROGRESS");
            drawPanel(window, GRID_MARGIN_X + 440, 840, 280, 40, "LEGEND");

            // --- RIGHT COLUMN ---
            drawPanel(window, RIGHT_COL_X, 20, 310, 400, "SYSTEM LOGS");
            float logY = 50;
            for(int i = std::max(0, (int)logger.messages.size() - 20); i < logger.messages.size(); ++i) {
                sf::Color lc = sf::Color(160, 180, 200);
                if(logger.messages[i].find("[WARN]") != string::npos) lc = sf::Color(240, 120, 60);
                else if(logger.messages[i].find("[ERROR]") != string::npos) lc = sf::Color(240, 60, 60);
                else if(logger.messages[i].find("Robot 1") != string::npos) lc = sf::Color(80, 200, 80);
                else if(logger.messages[i].find("Robot 2") != string::npos) lc = sf::Color(60, 140, 240);
                
                drawClippedText(logger.messages[i], 12, sf::Vector2f(RIGHT_COL_X+15, logY), lc, 280);
                logY += 18;
            }

            drawPanel(window, RIGHT_COL_X, 440, 310, 260, "ENERGY LEVELS");
            for(size_t i = 0; i < robots.size(); ++i) {
                sf::RectangleShape barBg(sf::Vector2f(20, 180));
                barBg.setPosition(sf::Vector2f(RIGHT_COL_X + 30 + i*45, 480));
                barBg.setFillColor(sf::Color(25, 35, 45));
                window.draw(barBg);
                
                float eH = 180 * (robots[i]->getEnergy() / 100.0f);
                sf::RectangleShape bar(sf::Vector2f(20, eH));
                bar.setPosition(sf::Vector2f(RIGHT_COL_X + 30 + i*45, 480 + (180 - eH)));
                bar.setFillColor(robots[i]->getSymbol() == 'E' ? sf::Color(80, 200, 80) : sf::Color(60, 140, 240));
                window.draw(bar);
                
                drawClippedText("R" + numberToString(i+1), 12, sf::Vector2f(RIGHT_COL_X + 32 + i*45, 670), sf::Color(180,200,220), 40);
                drawClippedText(numberToString(robots[i]->getEnergy())+"%", 11, sf::Vector2f(RIGHT_COL_X + 28 + i*45, 465), sf::Color::White, 40);
            }

            drawPanel(window, RIGHT_COL_X, 720, 310, 160, "CONTROLS");
            sf::RectangleShape startBtn(sf::Vector2f(80, 35));
            startBtn.setFillColor(sf::Color(60, 160, 60));
            startBtn.setPosition(sf::Vector2f(RIGHT_COL_X + 20, 760));
            window.draw(startBtn);
            drawClippedText("Start", 14, sf::Vector2f(RIGHT_COL_X + 40, 768), sf::Color::White, 80);
            
            sf::RectangleShape pauseBtn(sf::Vector2f(80, 35));
            pauseBtn.setFillColor(sf::Color(200, 120, 40));
            pauseBtn.setPosition(sf::Vector2f(RIGHT_COL_X + 115, 760));
            window.draw(pauseBtn);
            drawClippedText("Pause", 14, sf::Vector2f(RIGHT_COL_X + 130, 768), sf::Color::White, 80);

            sf::RectangleShape stopBtn(sf::Vector2f(80, 35));
            stopBtn.setFillColor(sf::Color(200, 60, 60));
            stopBtn.setPosition(sf::Vector2f(RIGHT_COL_X + 210, 760));
            window.draw(stopBtn);
            drawClippedText("Stop", 14, sf::Vector2f(RIGHT_COL_X + 235, 768), sf::Color::White, 80);

            window.display();
        }
    }
'''
with open('new_sg.txt', 'w', encoding='utf-8') as f:
    f.write(cpp_code)
print("File new_sg.txt generated.")
