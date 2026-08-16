    void startGraphics()
    {

        const int SIZE = 10;
        const int CELL = 48;
        const int MARGIN = 10;
        const int gridPixel = SIZE * CELL;
        const int winW = gridPixel + 380;
        const int winH = gridPixel + 20;

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
        if(!useTextures) logger.info("One or more sprite files missing; using procedural shapes instead.");

        sf::Font font;
        if(!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
        {
            logger.error("Failed to load font C:/Windows/Fonts/arial.ttf");
        }

        sf::Clock clock;
        float updateInterval = 0.5f; // seconds between simulation steps
        float accumulator = 0.0f;
        float animTime = 0.0f;

        auto drawRobotFallback = [&](sf::RenderWindow& window,
                                     const sf::Vector2f& pos,
                                     bool collector,
                                     float size)
        {
            // Draw a stylized robot with subtle glow/halo and a clear center icon
            sf::Color bodyColor = collector ? sf::Color(80, 200, 120) : sf::Color(90, 140, 220);
            sf::Color accent = collector ? sf::Color(255, 220, 90) : sf::Color(140, 200, 255);
            sf::Color dark = sf::Color(18, 22, 30);

            float bodyW = size * 0.56f;
            float bodyH = size * 0.44f;
            float headW = size * 0.34f;
            float headH = size * 0.22f;

            // halo / glow
            sf::CircleShape halo(bodyW * 0.6f);
            int haloA = 80 + int(40.0f * (std::sin(animTime * 3.0f) + 1.0f) / 2.0f);
            halo.setFillColor(sf::Color(bodyColor.r, bodyColor.g, bodyColor.b, haloA));
            halo.setOrigin(sf::Vector2f(halo.getRadius(), halo.getRadius()));
            halo.setPosition(sf::Vector2f(pos.x, pos.y));
            window.draw(halo);

            // body
            sf::RectangleShape body(sf::Vector2f(bodyW, bodyH));
            body.setFillColor(bodyColor);
            body.setOutlineThickness(2.0f);
            body.setOutlineColor(dark);
            body.setOrigin(sf::Vector2f(bodyW/2.0f, bodyH/2.0f));
            body.setPosition(sf::Vector2f(pos.x, pos.y));
            window.draw(body);

            // chest panel
            sf::RectangleShape chest(sf::Vector2f(bodyW * 0.56f, bodyH * 0.46f));
            chest.setFillColor(sf::Color(240,245,250));
            chest.setOutlineThickness(1.2f);
            chest.setOutlineColor(dark);
            chest.setOrigin(sf::Vector2f(chest.getSize().x/2.0f, chest.getSize().y/2.0f));
            chest.setPosition(sf::Vector2f(pos.x, pos.y + bodyH*0.02f));
            window.draw(chest);

            // head
            sf::RectangleShape head(sf::Vector2f(headW, headH));
            head.setFillColor(sf::Color(230,235,240));
            head.setOutlineThickness(1.8f);
            head.setOutlineColor(dark);
            head.setOrigin(sf::Vector2f(headW/2.0f, headH/2.0f));
            head.setPosition(sf::Vector2f(pos.x, pos.y - bodyH*0.62f));
            window.draw(head);

            // visor
            sf::RectangleShape visor(sf::Vector2f(headW * 0.66f, headH * 0.44f));
            visor.setFillColor(sf::Color(40,80,140));
            visor.setOrigin(sf::Vector2f(visor.getSize().x/2.0f, visor.getSize().y/2.0f));
            visor.setPosition(sf::Vector2f(pos.x, pos.y - bodyH*0.62f + 2.0f));
            window.draw(visor);

            // antenna
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

            // arms
            sf::RectangleShape leftArm(sf::Vector2f(6.0f, 18.0f));
            leftArm.setFillColor(dark);
            leftArm.setOrigin(sf::Vector2f(3.0f, 0.0f));
            leftArm.setPosition(sf::Vector2f(pos.x - bodyW*0.52f, pos.y - bodyH*0.15f));
            window.draw(leftArm);

            sf::RectangleShape rightArm = leftArm;
            rightArm.setOrigin(sf::Vector2f(3.0f, 0.0f));
            rightArm.setPosition(sf::Vector2f(pos.x + bodyW*0.52f, pos.y - bodyH*0.15f));
            window.draw(rightArm);

            // legs
            sf::RectangleShape lLeg(sf::Vector2f(7.0f, 14.0f));
            lLeg.setFillColor(dark);
            lLeg.setOrigin(sf::Vector2f(3.5f, 0.0f));
            lLeg.setPosition(sf::Vector2f(pos.x - bodyW*0.18f, pos.y + bodyH*0.46f));
            window.draw(lLeg);

            sf::RectangleShape rLeg = lLeg;
            rLeg.setPosition(sf::Vector2f(pos.x + bodyW*0.18f, pos.y + bodyH*0.46f));
            window.draw(rLeg);

            // center icon (simple shape to differentiate types)
            if(collector)
            {
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
            }
            else
            {
                sf::CircleShape core(6.0f);
                core.setFillColor(sf::Color(240,250,255));
                core.setOrigin(sf::Vector2f(core.getRadius(), core.getRadius()));
                core.setPosition(sf::Vector2f(pos.x, pos.y - bodyH*0.08f));
                window.draw(core);
            }
        };

        // Initialize render positions and trails for robots
        renderPositions.clear();
        trails.clear();
        renderPositions.resize(robots.size());
        trails.resize(robots.size());
        for(size_t i = 0; i < robots.size(); i++)
        {
            float rx = MARGIN + robots[i]->getY() * CELL + CELL / 2.0f;
            float ry = MARGIN + robots[i]->getX() * CELL + CELL / 2.0f;
            renderPositions[i] = sf::Vector2f(rx, ry);
            trails[i].push_back(sf::Vector2f((float)robots[i]->getX(), (float)robots[i]->getY()));
        }

        while(window.isOpen())
        {
            // poll events (SFML3 uses optional<Event>); iterate but avoid accessing incompatible members
            while(true)
            {
                auto ev = window.pollEvent();
                if(!ev) break;
                // fallback: allow Escape key to close
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

            // Rendering
            window.clear(sf::Color(18, 24, 31));

            // UI button rects
            float panelX = float(MARGIN + gridPixel + 12);
            float panelY = float(MARGIN);
            // place buttons slightly lower to avoid header/summary overlap
            float buttonY = panelY + 64.0f;
            sf::FloatRect startRect(sf::Vector2f(panelX + 20.0f, buttonY), sf::Vector2f(80.0f, 36.0f));
            sf::FloatRect pauseRect(sf::Vector2f(panelX + 110.0f, buttonY), sf::Vector2f(80.0f, 36.0f));
            sf::FloatRect stopRect(sf::Vector2f(panelX + 200.0f, buttonY), sf::Vector2f(80.0f, 36.0f));

            // Draw grid background as a single VertexArray for performance
            static sf::VertexArray gridBg(sf::PrimitiveType::Triangles);
            if(gridBg.getVertexCount() == 0)
            {
                gridBg.clear();
                for(int i = 0; i < SIZE; ++i)
                {
                    for(int j = 0; j < SIZE; ++j)
                    {
                        float x = float(MARGIN + j * CELL);
                        float y = float(MARGIN + i * CELL);
                        float s = float(CELL - 2);
                        // two triangles forming a quad
                        {
                            sf::Vertex v;
                            v.color = sf::Color(22,30,36);
                            v.position = sf::Vector2f(x, y);
                            gridBg.append(v);
                            v.position = sf::Vector2f(x+s, y);
                            gridBg.append(v);
                            v.position = sf::Vector2f(x+s, y+s);
                            gridBg.append(v);

                            v.position = sf::Vector2f(x, y);
                            gridBg.append(v);
                            v.position = sf::Vector2f(x+s, y+s);
                            gridBg.append(v);
                            v.position = sf::Vector2f(x, y+s);
                            gridBg.append(v);
                        }
                    }
                }
            }
            window.draw(gridBg);

            // Draw grid lines (subtle)
            static sf::VertexArray gridLines(sf::PrimitiveType::Lines);
            if(gridLines.getVertexCount() == 0)
            {
                gridLines.clear();
                for(int i = 0; i <= SIZE; ++i)
                {
                    float y = float(MARGIN + i * CELL) - 1.0f;
                    {
                        sf::Vertex v;
                        v.position = sf::Vector2f(float(MARGIN), y);
                        v.color = sf::Color(24,40,34);
                        gridLines.append(v);
                        v.position = sf::Vector2f(float(MARGIN + SIZE*CELL), y);
                        gridLines.append(v);
                    }
                }
                for(int j = 0; j <= SIZE; ++j)
                {
                    float x = float(MARGIN + j * CELL) - 1.0f;
                    {
                        sf::Vertex v;
                        v.position = sf::Vector2f(x, float(MARGIN));
                        v.color = sf::Color(24,40,34);
                        gridLines.append(v);
                        v.position = sf::Vector2f(x, float(MARGIN + SIZE*CELL));
                        gridLines.append(v);
                    }
                }
            }
            window.draw(gridLines);

            // Draw resources
            for(size_t r = 0; r < resources.size(); r++)
            {
                if(resources[r].isCollected()) continue;

                float rx = MARGIN + resources[r].getY() * CELL + CELL / 2.0f;
                float ry = MARGIN + resources[r].getX() * CELL + CELL / 2.0f;

                if(useTextures)
                {
                    sf::Sprite rs(texResource);
                    float scaleR = (CELL * 0.8f) / float(texResource.getSize().x);
                    rs.setScale(sf::Vector2f(scaleR, scaleR));
                    rs.setOrigin(sf::Vector2f(rs.getLocalBounds().size.x/2.0f, rs.getLocalBounds().size.y/2.0f));
                    rs.setPosition(sf::Vector2f(rx, ry));
                    window.draw(rs);
                }
                else
                {
                    // draw star-shaped resource
                    sf::ConvexShape star;
                    star.setPointCount(10);
                    float R = CELL * 0.28f;
                    float r = R * 0.45f;
                    for(int p=0;p<10;p++){
                        float ang = (float)p * 2.0f * 3.14159265f / 10.0f - 3.14159265f/2.0f;
                        float rad = (p%2==0)?R:r;
                        star.setPoint(p, sf::Vector2f(rx + cos(ang)*rad, ry + sin(ang)*rad));
                    }
                    star.setFillColor(sf::Color(255,215,0));
                    star.setOutlineThickness(1.0f);
                    star.setOutlineColor(sf::Color(200,170,0));
                    window.draw(star);
                }
            }

            // Draw obstacles
            for(size_t o = 0; o < obstacles.size(); o++)
            {
                float ox = MARGIN + obstacles[o].getY() * CELL + CELL / 2.0f;
                float oy = MARGIN + obstacles[o].getX() * CELL + CELL / 2.0f;

                if(useTextures)
                {
                    sf::Sprite os(texObstacle);
                    float scaleO = (CELL * 0.9f) / float(texObstacle.getSize().x);
                    os.setScale(sf::Vector2f(scaleO, scaleO));
                    os.setOrigin(sf::Vector2f(os.getLocalBounds().size.x/2.0f, os.getLocalBounds().size.y/2.0f));
                    os.setPosition(sf::Vector2f(ox,oy));
                    window.draw(os);
                }
                else
                {
                    // draw small rock pile with overlapping circles
                    float r1 = CELL * 0.22f;
                    sf::CircleShape rock1(r1);
                    rock1.setFillColor(sf::Color(110,110,115));
                    rock1.setOutlineThickness(1.0f);
                    rock1.setOutlineColor(sf::Color(90,90,95));
                    rock1.setOrigin(sf::Vector2f(r1, r1));
                    rock1.setPosition(sf::Vector2f(ox - r1*0.2f, oy - r1*0.1f));
                    window.draw(rock1);

                    sf::CircleShape rock2(r1*0.8f);
                    rock2.setFillColor(sf::Color(120,120,125));
                    rock2.setOutlineThickness(1.0f);
                    rock2.setOutlineColor(sf::Color(95,95,100));
                    rock2.setOrigin(sf::Vector2f(rock2.getRadius(), rock2.getRadius()));
                    rock2.setPosition(sf::Vector2f(ox + r1*0.2f, oy - r1*0.05f));
                    window.draw(rock2);

                    sf::CircleShape rock3(r1*0.6f);
                    rock3.setFillColor(sf::Color(100,100,105));
                    rock3.setOutlineThickness(1.0f);
                    rock3.setOutlineColor(sf::Color(80,80,85));
                    rock3.setOrigin(sf::Vector2f(rock3.getRadius(), rock3.getRadius()));
                    rock3.setPosition(sf::Vector2f(ox, oy + r1*0.25f));
                    window.draw(rock3);
                }
            }

            // Draw trails and robots with smooth animation
            for(size_t i = 0; i < robots.size(); i++)
            {
                Robot* rb = robots[i];

                // draw trail (grid->pixel) as dashed, with glow
                if(trails[i].size() >= 2)
                {
                    sf::Color trailColor = (rb->getSymbol() == 'C') ? sf::Color(120,210,140) : sf::Color(120,160,240);
                    for(size_t t = 1; t < trails[i].size(); t++)
                    {
                        float x1 = MARGIN + trails[i][t-1].y * CELL + CELL / 2.0f;
                        float y1 = MARGIN + trails[i][t-1].x * CELL + CELL / 2.0f;
                        float x2 = MARGIN + trails[i][t].y * CELL + CELL / 2.0f;
                        float y2 = MARGIN + trails[i][t].x * CELL + CELL / 2.0f;
                        // split into short segments for dashed look
                        const int SEG = 6;
                        for(int s=0;s<SEG;s++){
                            float u1 = float(s)/float(SEG);
                            float u2 = float(s+0.5f)/float(SEG);
                            float sx1 = x1 + (x2-x1)*u1;
                            float sy1 = y1 + (y2-y1)*u1;
                            float sx2 = x1 + (x2-x1)*u2;
                            float sy2 = y1 + (y2-y1)*u2;
                            sf::Vertex segv[2];
                            segv[0].position = sf::Vector2f(sx1, sy1);
                            segv[1].position = sf::Vector2f(sx2, sy2);
                            segv[0].color = sf::Color(trailColor.r, trailColor.g, trailColor.b, 200);
                            segv[1].color = sf::Color(trailColor.r, trailColor.g, trailColor.b, 200);
                            window.draw(segv, 2, sf::PrimitiveType::Lines);
                        }
                        // draw small arrowhead at 75% along the segment
                        float ax = x1 + (x2 - x1) * 0.75f;
                        float ay = y1 + (y2 - y1) * 0.75f;
                        float ang = std::atan2(y2 - y1, x2 - x1);
                        float s = 6.0f;
                        sf::ConvexShape tri;
                        tri.setPointCount(3);
                        tri.setPoint(0, sf::Vector2f(ax + std::cos(ang) * s, ay + std::sin(ang) * s));
                        tri.setPoint(1, sf::Vector2f(ax - std::cos(ang) * s + std::sin(ang) * s * 0.5f,
                                                   ay - std::sin(ang) * s - std::cos(ang) * s * 0.5f));
                        tri.setPoint(2, sf::Vector2f(ax - std::cos(ang) * s - std::sin(ang) * s * 0.5f,
                                                   ay - std::sin(ang) * s + std::cos(ang) * s * 0.5f));
                        tri.setFillColor(sf::Color(trailColor.r, trailColor.g, trailColor.b, 220));
                        tri.setOutlineThickness(0.8f);
                        tri.setOutlineColor(sf::Color(20,20,20,160));
                        window.draw(tri);
                    }
                }

                // smooth lerp render position towards actual robot cell center
                sf::Vector2f target(MARGIN + rb->getY() * CELL + CELL / 2.0f,
                                    MARGIN + rb->getX() * CELL + CELL / 2.0f);
                float alpha = std::min(1.0f, dt * 8.0f);
                renderPositions[i].x += (target.x - renderPositions[i].x) * alpha;
                renderPositions[i].y += (target.y - renderPositions[i].y) * alpha;

                // draw robot sprite or fallback shape
                if(useTextures)
                {
                    sf::Sprite spr = (rb->getSymbol() == 'C') ? sf::Sprite(texCollector) : sf::Sprite(texExplorer);
                    float texW = (rb->getSymbol() == 'C') ? float(texCollector.getSize().x) : float(texExplorer.getSize().x);
                    float scaleS = (CELL * 0.9f) / texW;
                    spr.setScale(sf::Vector2f(scaleS, scaleS));
                    spr.setOrigin(sf::Vector2f(spr.getLocalBounds().size.x/2.0f, spr.getLocalBounds().size.y/2.0f));
                    spr.setPosition(renderPositions[i]);
                    window.draw(spr);
                }
                else
                {
                    drawRobotFallback(window, renderPositions[i], rb->getSymbol() == 'C', CELL * 0.72f);
                }

                // ID text
                sf::Text idText(font, numberToString(rb->getID()), 14);
                idText.setFillColor(sf::Color::White);
                idText.setPosition(sf::Vector2f(renderPositions[i].x - 6.0f, renderPositions[i].y - 10.0f));
                window.draw(idText);
            }

            // Right-side telemetry panel background
            sf::RectangleShape panel(sf::Vector2f(340, (float)gridPixel));
            panel.setPosition(sf::Vector2f(float(MARGIN + gridPixel + 12), float(MARGIN)));
            panel.setFillColor(sf::Color(12, 18, 24));
            panel.setOutlineThickness(1);
            panel.setOutlineColor(sf::Color(50, 60, 70));
            window.draw(panel);

            // Draw base / charging station on grid as sprite or shape
            float bx = MARGIN + basePos.second * CELL + CELL / 2.0f;
            float by = MARGIN + basePos.first * CELL + CELL / 2.0f;
            if(useTextures)
            {
                sf::Sprite bs(texBase);
                float scaleB = (CELL * 1.0f) / float(texBase.getSize().x);
                bs.setScale(sf::Vector2f(scaleB, scaleB));
                bs.setOrigin(sf::Vector2f(bs.getLocalBounds().size.x/2.0f, bs.getLocalBounds().size.y/2.0f));
                bs.setPosition(sf::Vector2f(bx, by));
                window.draw(bs);
            }
            else
            {
                sf::RectangleShape baseShape(sf::Vector2f(CELL * 0.9f, CELL * 0.9f));
                baseShape.setOrigin(sf::Vector2f(baseShape.getSize().x/2.0f, baseShape.getSize().y/2.0f));
                baseShape.setPosition(sf::Vector2f(bx, by));
                baseShape.setFillColor(sf::Color(140,80,200));
                window.draw(baseShape);
            }

            // Telemetry header
            sf::Text header(font, "TELEMETRY", 20);
            header.setFillColor(sf::Color(160, 220, 255));
            header.setPosition(sf::Vector2f(panelX + 12.0f, panelY + 8.0f));
            window.draw(header);

            // Helper: draw text clipped to a maximum width (appends ... if truncated)
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

            // Telemetry summary (step / counts / running)
            float panelInnerW = 340.0f - 24.0f;
            drawClippedText(string("Step: ") + numberToString(currentStep) + " / " + numberToString(maxSteps), 14, sf::Vector2f(panelX + 14.0f, panelY + 36.0f), sf::Color(200,200,220), panelInnerW * 0.45f);
            drawClippedText(string("Robots: ") + numberToString((int)robots.size()), 14, sf::Vector2f(panelX + 14.0f + panelInnerW * 0.45f + 8.0f, panelY + 36.0f), sf::Color(200,200,220), panelInnerW * 0.25f);
            drawClippedText(string("Running: ") + (running?"Yes":"No"), 14, sf::Vector2f(panelX + 14.0f + panelInnerW * 0.72f + 8.0f, panelY + 36.0f), running?sf::Color(140,240,160):sf::Color(200,180,160), panelInnerW * 0.27f);

            // Global statistics small block (placed under buttons)
            float gsX = panelX + 12.0f;
            float gsBoxH = 64.0f;
            float gsY = buttonY + 44.0f; // just below buttons
            sf::RectangleShape gsBox(sf::Vector2f(156.0f, gsBoxH)); gsBox.setPosition(sf::Vector2f(gsX, gsY)); gsBox.setFillColor(sf::Color(18,26,30)); gsBox.setOutlineThickness(1.0f); gsBox.setOutlineColor(sf::Color(40,50,60)); window.draw(gsBox);
            int robotsActive = (int)robots.size();
            int resourcesFound = initialResourceCount;
            int resourcesCollected = 0; for(auto &r: resources) if(r.isDelivered()) resourcesCollected++;
            int resourcesRemaining = 0; for(auto &r: resources) if(!r.isCollected()) resourcesRemaining++;
            drawClippedText(string("Robots Active: ") + numberToString(robotsActive), 12, sf::Vector2f(gsX + 8.0f, gsY + 8.0f), sf::Color(200,220,220), 140.0f);
            drawClippedText(string("Resources: ") + numberToString(resourcesCollected) + string("/") + numberToString(resourcesFound), 12, sf::Vector2f(gsX + 8.0f, gsY + 26.0f), sf::Color(200,220,220), 140.0f);
            drawClippedText(string("Total Dist: ") + numberToString(totalDistance), 12, sf::Vector2f(gsX + 140.0f, gsY + 26.0f), sf::Color(160,200,180), 100.0f);

            // Draw control buttons
            sf::RectangleShape startBtn(sf::Vector2f(startRect.size.x, startRect.size.y));
            startBtn.setPosition(sf::Vector2f(startRect.position.x, startRect.position.y));
            startBtn.setFillColor(running && !paused ? sf::Color(40,160,40) : sf::Color(60,200,80));
            window.draw(startBtn);
            sf::Text startText(font, "Start", 16);
            startText.setFillColor(sf::Color::Black);
            startText.setPosition(sf::Vector2f(startRect.position.x + (startRect.size.x- startText.getLocalBounds().size.x)/2.0f - 6.0f, startRect.position.y + 8));
            window.draw(startText);

            sf::RectangleShape pauseBtn(sf::Vector2f(pauseRect.size.x, pauseRect.size.y));
            pauseBtn.setPosition(sf::Vector2f(pauseRect.position.x, pauseRect.position.y));
            pauseBtn.setFillColor(paused ? sf::Color(200,160,40) : sf::Color(200,200,80));
            window.draw(pauseBtn);
            sf::Text pauseText(font, "Pause", 16);
            pauseText.setFillColor(sf::Color::Black);
            pauseText.setPosition(sf::Vector2f(pauseRect.position.x + (pauseRect.size.x- pauseText.getLocalBounds().size.x)/2.0f - 6.0f, pauseRect.position.y + 8));
            window.draw(pauseText);

            sf::RectangleShape stopBtn(sf::Vector2f(stopRect.size.x, stopRect.size.y));
            stopBtn.setPosition(sf::Vector2f(stopRect.position.x, stopRect.position.y));
            stopBtn.setFillColor(!running ? sf::Color(180,40,40) : sf::Color(220,80,80));
            window.draw(stopBtn);
            sf::Text stopText(font, "Stop", 16);
            stopText.setFillColor(sf::Color::Black);
            stopText.setPosition(sf::Vector2f(stopRect.position.x + (stopRect.size.x- stopText.getLocalBounds().size.x)/2.0f - 6.0f, stopRect.position.y + 8));
            window.draw(stopText);

            // Mouse click handling (edge detect)
            bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            auto mp = sf::Mouse::getPosition(window);
            if(mousePressed && !mousePrevPressed)
            {
                sf::Vector2f mfp((float)mp.x, (float)mp.y);
                if(sf::FloatRect(startRect).contains(mfp)) { running = true; paused = false; }
                else if(sf::FloatRect(pauseRect).contains(mfp)) { if(running) paused = !paused; }
                else if(sf::FloatRect(stopRect).contains(mfp)) { running = false; paused = false; }
            }
            mousePrevPressed = mousePressed;

            // Robot telemetry list (with colored status dots)
            float panelW = 340.0f;
            float contentTop = gsY + gsBoxH + 8.0f; // below global stats
            float contentHeight = (float)gridPixel - (contentTop - panelY) - 120.0f; if(contentHeight < 60.0f) contentHeight = 60.0f;
            float listX = panelX + 14.0f;
            float listY = contentTop + 8.0f;
            float lineH = 18.0f;
            int maxShow = std::max(3, std::min(20, (int)std::floor((contentHeight - 12.0f)/lineH)));
            float logW = 140.0f;
            float logX = panelX + panelW - logW - 12.0f;
            float logY = contentTop + 4.0f;
            float logH = contentHeight - 8.0f;
            for(size_t i = 0; i < robots.size() && (int)i < maxShow; i++)
            {
                Robot* rb = robots[i];
                string status = rb->getStatus();
                string line = "R" + numberToString(rb->getID()) + ": (" + numberToString(rb->getX()) + "," + numberToString(rb->getY()) + ") ";
                string full = line + status;
                // truncate long lines to avoid overflow
                if(full.size() > 48) full = full.substr(0,45) + "...";
                sf::Text t(font, full, 14);
                t.setFillColor(sf::Color::White);
                float maxLineW = logX - (listX + 34.0f);
                // draw clipped text using helper to ensure it doesn't go under logs
                drawClippedText(full, 14, sf::Vector2f(listX + 18.0f, listY + i * 20.0f), sf::Color::White, maxLineW);

                // status color heuristics
                sf::Color stCol = sf::Color(150,150,150);
                string sLow = status;
                transform(sLow.begin(), sLow.end(), sLow.begin(), ::tolower);
                if(sLow.find("idle") != string::npos) stCol = sf::Color(160,160,160);
                else if(sLow.find("deliver") != string::npos || sLow.find("delivered") != string::npos) stCol = sf::Color(120,220,140);
                else if(sLow.find("collect") != string::npos) stCol = sf::Color(240,200,80);
                else if(sLow.find("charge") != string::npos) stCol = sf::Color(120,200,240);
                else stCol = sf::Color(140,180,240);

                // draw colored dot
                sf::CircleShape dot(6);
                dot.setFillColor(stCol);
                dot.setOrigin(sf::Vector2f(6,6));
                dot.setPosition(sf::Vector2f(listX + 8.0f, listY + i * 20.0f + 10.0f));
                window.draw(dot);

                // window.draw(t); handled by drawClippedText
            }
            if(robots.size() > (size_t)maxShow)
            {
                sf::Text more(font, string("+") + numberToString((int)robots.size() - maxShow) + " more...", 14);
                more.setFillColor(sf::Color(150,150,150)); more.setPosition(sf::Vector2f(listX + 18.0f, listY + maxShow * lineH)); window.draw(more);
            }

            // Energy levels (top of right column) and System logs below
            float energyH = std::min(120.0f, logH * 0.45f);
            float logsH = logH - energyH - 8.0f; if(logsH < 48.0f) { logsH = std::max(48.0f, logH * 0.4f); energyH = logH - logsH - 8.0f; }
            sf::RectangleShape energyBox(sf::Vector2f(logW, energyH)); energyBox.setPosition(sf::Vector2f(logX, logY)); energyBox.setFillColor(sf::Color(10,14,18)); energyBox.setOutlineThickness(1.0f); energyBox.setOutlineColor(sf::Color(40,50,60)); window.draw(energyBox);
            sf::Text eHeader(font, "ENERGY LEVELS", 12); eHeader.setFillColor(sf::Color(140,200,140)); eHeader.setPosition(sf::Vector2f(logX + 8.0f, logY + 6.0f)); window.draw(eHeader);
            // draw vertical bars for each robot
            int rn = (int)robots.size(); if(rn > 0){ float barAreaW = logW - 16.0f; float per = std::min(36.0f, std::max(8.0f, (barAreaW - (rn-1)*4.0f)/ (float)rn)); float startX = logX + 8.0f + (barAreaW - (per*rn + (rn-1)*4.0f))/2.0f; float barMaxH = energyH - 40.0f; for(int i=0;i<rn;i++){ Robot* rbt = robots[i]; float ex = startX + i * (per + 4.0f); float val = (float)rbt->getEnergy() / 100.0f; sf::RectangleShape barBg(sf::Vector2f(per, barMaxH)); barBg.setPosition(sf::Vector2f(ex, logY + 24.0f)); barBg.setFillColor(sf::Color(30,36,32)); window.draw(barBg); sf::RectangleShape barFill(sf::Vector2f(per, barMaxH * val)); barFill.setPosition(sf::Vector2f(ex, logY + 24.0f + (barMaxH * (1.0f - val)))); // color gradient
                        sf::Color bc = (val > 0.75f) ? sf::Color(80,220,100) : (val > 0.4f ? sf::Color(240,200,80) : sf::Color(220,80,80)); barFill.setFillColor(bc); window.draw(barFill);
                        sf::Text pct(font, numberToString((int)std::round(val*100.0f)) + "%", 10); pct.setFillColor(sf::Color(200,200,200)); pct.setPosition(sf::Vector2f(ex + per*0.1f, logY + 6.0f)); window.draw(pct);
                        // robot label
                        sf::Text rlab(font, string("R") + numberToString(robots[i]->getID()), 10); rlab.setFillColor(sf::Color(180,200,180)); rlab.setPosition(sf::Vector2f(ex + per*0.12f, logY + 24.0f + barMaxH + 2.0f)); window.draw(rlab);
            }}
            // logs box under energy
            sf::RectangleShape logBox(sf::Vector2f(logW, logsH)); logBox.setPosition(sf::Vector2f(logX, logY + energyH + 8.0f)); logBox.setFillColor(sf::Color(10,14,18)); logBox.setOutlineThickness(1.0f); logBox.setOutlineColor(sf::Color(40,50,60)); window.draw(logBox);
            sf::Text logHeader(font, "SYSTEM LOGS", 12); logHeader.setFillColor(sf::Color(140,200,240)); logHeader.setPosition(sf::Vector2f(logX + 8.0f, logY + energyH + 12.0f)); window.draw(logHeader);
            int maxLogs = std::max(2, (int)std::floor((logsH - 28.0f)/16.0f)); int start = std::max(0, (int)g_logs.size() - maxLogs);
            for(int i = 0; i < maxLogs && start + i < (int)g_logs.size(); ++i){ string s = g_logs[start + i]; float maxLogW = logW - 16.0f; drawClippedText(s, 11, sf::Vector2f(logX + 8.0f, logY + energyH + 32.0f + i*16.0f), sf::Color(160,200,220), maxLogW); }

            // Resources left (summary area)
            int remaining = 0;
            for(size_t i = 0; i < resources.size(); i++) if(!resources[i].isCollected()) remaining++;
            sf::Text resText(font, "Resources Left: " + numberToString(remaining), 16);
            resText.setFillColor(sf::Color(200, 230, 200));
            resText.setPosition(sf::Vector2f(panelX + 14.0f, panelY + (float)gridPixel - 42.0f));
            window.draw(resText);

            // Mission progress bar (bottom of panel)
            int total = (int)resources.size();
            int delivered = 0;
            for(size_t i=0;i<resources.size();++i) if(resources[i].isDelivered()) delivered++;
            float progress = total>0 ? float(delivered) / float(total) : 0.0f;
            float progW = panelW - 52.0f; float progH = 18.0f;
            sf::RectangleShape progBg(sf::Vector2f(progW, progH));
            float progY = panelY + (float)gridPixel - 64.0f; // leave some bottom margin
            progBg.setPosition(sf::Vector2f(panelX + 14.0f, progY));
            progBg.setFillColor(sf::Color(30,40,36));
            window.draw(progBg);
            sf::RectangleShape progBar(sf::Vector2f(progW * progress, progH));
            progBar.setPosition(progBg.getPosition());
            progBar.setFillColor(sf::Color(60,200,120));
            window.draw(progBar);
            sf::Text progText(font, "Collected: " + numberToString(delivered) + " / " + numberToString(total), 14);
            progText.setFillColor(sf::Color(220,220,220));
            progText.setPosition(sf::Vector2f(progBg.getPosition().x + 6, progBg.getPosition().y - 2));
            window.draw(progText);

            // Legend (small icons) -- place above bottom progress area
            float lx = float(panelX + 28.0f);
            float ly = float(progBg.getPosition().y + progBg.getSize().y + 8.0f);
            // Explorer icon
            drawRobotFallback(window, sf::Vector2f(lx + 10.0f, ly + 8.0f), false, 18.0f);
            sf::Text explText(font, "Explorer Robot", 12);
            explText.setFillColor(sf::Color(200,200,200));
            explText.setPosition(sf::Vector2f(lx + 22, ly - 2));
            window.draw(explText);

            // Collector icon
            drawRobotFallback(window, sf::Vector2f(lx + 10.0f, ly + 28.0f), true, 18.0f);
            sf::Text collText(font, "Collector Robot", 12);
            collText.setFillColor(sf::Color(200,200,200));
            collText.setPosition(sf::Vector2f(lx + 22, ly + 18));
            window.draw(collText);

            // Resource star icon
            sf::CircleShape starIcon(6,6);
            starIcon.setFillColor(sf::Color(255,215,0));
            starIcon.setPosition(sf::Vector2f(lx, ly + 40));
            window.draw(starIcon);
            sf::Text resText2(font, "Resource", 12);
            resText2.setFillColor(sf::Color(200,200,200));
            resText2.setPosition(sf::Vector2f(lx + 22, ly + 38));
            window.draw(resText2);

            window.display();
        }

        cout << "Simulation Finished!" << endl;

    }

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