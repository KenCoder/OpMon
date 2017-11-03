#include "Overworld.hpp"
#include "../start/Initializer.hpp"
#include "../start/main.hpp"
#define FPS_TICKS 33
#include <cmath>
//#define ppPosY ((character.getPosition().y / CASE_SIZE) - 8)
//#define ppPosX (((character.getPosition().x - 16) / CASE_SIZE) - 8)
#include "Events.hpp"
#include "Dialog.hpp"
					      
#ifndef _WIN32

//#define DEBUG_REPORT

#endif

UNS

void Overworld::initVars() {
  
}

void Overworld::tp(std::string toTp, sf::Vector2i pos){
  Model::Data::player.getPosition().tp(toTp, pos);
  actual = &Model::Data::World::maps.at(Model::Data::player.getPosition().getMapId());
  character.setPosition(8 CASES + pos.x CASES - 16, 8 CASES + pos.y CASES);
  if(music != actual->getBg()) {
    music->stop();
    music = actual->getBg();
    music->play();
  }

  delete(layer1);
  delete(layer2);
  delete(layer3);
  
  layer1 = new MapLayer(actual->getLayer1());
  layer2 = new MapLayer(actual->getLayer2());
  layer3 = new MapLayer(actual->getLayer3());
  tpCount = 0;
}



GameStatus Overworld::overworld() {
    for(auto map = Initializer::maps.cbegin(); map!=Initializer::maps.cend(); ++map) {
        for(Event *event : map->second.getEvents()) {
            Events::TalkingEvent *te = dynamic_cast<Events::TalkingEvent *>(event);
            if(te != nullptr) {
                te->reloadKeys();
            }
        }
    }

    music->play();
    Window::frame.setView(camera);
    GameStatus returned = boucle();
    music->stop();
    delete(layer1);
    delete(layer2);
    delete(layer3);
    return returned;
}

GameStatus Overworld::boucle() {
    bool continuer = true;
    while(continuer) {
        if((GET_TICKS - ancientTick >= FPS_TICKS)) {
	   fpsCounter++;
            if(GET_TICKS - oldTicksFps >= 1000) {
                fps = "";
                fps << fpsCounter;
                fpsPrint.setString(fps);
                fpsCounter = 0;
		oldTicksFps = GET_TICKS;
            }
            frames++;
	    if(debugMode){
	      cout << "[FRAME Nｰ" << frames << "]" << endl;
	      cout << "Boucle : Normal" << endl;
	      cout << "Tick: " << ticks.getElapsedTime().asMilliseconds() << "ms" << endl;
	      cout << "PlayerPosition: " << ppPosX << " - " << ppPosY << endl;
	      cout << "PlayerPositionPx: " << character.getPosition().x << " - " << character.getPosition().y << endl;
	      cout << "Moving: " << (int) moving << endl;
	      cout << "Anim: " << (int) anim << endl;
	      cout << "PlayerDirection: " << (int) ppDir << endl;
	      cout << "DebugMode: " << debugMode << endl;
	      cout << "MapPos: " << layer1->getPosition().x << " - " << layer1->getPosition().y << endl;
	      //cout << "Position perso : P(" << ppPosX << ";" << ppPosY << ")" << endl;
	    }
	    
            if(justTp) {
                tpCount++;
                justTp = tpCount < 0;
            }

            ancientTick = GET_TICKS;
	    Main::mainframe.window.pollEvent(Main::mainframe.events);

            switch(Main::mainframe.events.type) {
	      RETURN_ON_CLOSE_EVENT

            case sf::Event::KeyPressed:
                if(Main::mainframe.events.key.code == sf::Keyboard::Equal) {
                    debugMode = !debugMode;
                }
                if(debugMode) {
                    if(Main::mainframe.events.key.code == sf::Keyboard::F10) {
                        printlayer[0] = !printlayer[0];
                    }
                    if(Main::mainframe.events.key.code == sf::Keyboard::F11) {
                        printlayer[1] = !printlayer[1];
                    }
                    if(Main::mainframe.events.key.code == sf::Keyboard::F12) {
                        printlayer[2] = !printlayer[2];
                    }

                    if(Main::mainframe.events.key.code == sf::Keyboard::F5) {
		      tp("Mom's room", sf::Vector2i(0, 1), true);
                    } else if(Main::mainframe.events.key.code == sf::Keyboard::F6) {
		      tp("Player's room", sf::Vector2i(0, 0), true);
                    } else if(Main::mainframe.events.key.code == sf::Keyboard::F1) {
		      tp("Fauxbourg Euvi", sf::Vector2i(25, 28), true);
                    } else if(Main::mainframe.events.key.code == sf::Keyboard::F2) {
		      tp("Player's home", sf::Vector2i(8, 14), true);
                    } else if(Main::mainframe.events.key.code == sf::Keyboard::F3) {
		      tp("Laboratory", sf::Vector2i(15, 14), true);
                    } else if(Main::mainframe.events.key.code == sf::Keyboard::F4) {
		      tp("Rival's house", sf::Vector2i(8, 14), true);
                    } else if(Main::mainframe.events.key.code == sf::Keyboard::F7){
		      tp("Route 14", sf::Vector2i(0, 31), true);
		    } else if(Main::mainframe.events.key.code == sf::Keyboard::F8){
		      tp("MysteriouCity", sf::Vector2i(12, 0), true);
		    }
                }
            default:
                break;
            }
            RETURN_ON_ECHAP_EVENT
            if(Main::player.gameIsOver) {
                return -1;
            }
            if(!justTp) {
                if(isKeyPressed(sf::Keyboard::Up)) {
                    up();
                } else if(isKeyPressed(sf::Keyboard::Down)) {
                    down();
                } else if(isKeyPressed(sf::Keyboard::Left)) {
                    left();
                } else if(isKeyPressed(sf::Keyboard::Right)) {
                    right();
                }
            }
            sf::Text debugText;
            if(debugMode) {
                if(isKeyPressed(sf::Keyboard::Numpad2)) {
                    camera.move(0, 4);
                }
                if(isKeyPressed(sf::Keyboard::Numpad4)) {
                    camera.move(-4, 0);
                }
                if(isKeyPressed(sf::Keyboard::Numpad8)) {
                    camera.move(0, -4);
                }
                if(isKeyPressed(sf::Keyboard::Numpad6)) {
                    camera.move(4, 0);
                }
                debugText.setString("Debug mode");
                debugText.setPosition(Main::mainframe.frame.mapPixelToCoords(sf::Vector2i(0, 0)));
                debugText.setFont(Main::mainframe.font);
                debugText.setColor(sf::Color(127, 127, 127));
                debugText.setCharacterSize(40);
                fpsPrint.setPosition(Main::mainframe.frame.mapPixelToCoords(sf::Vector2i(0, 50)));
                fpsPrint.setFont(Main::mainframe.font);
                fpsPrint.setCharacterSize(48);
            }

            Main::mainframe.frame.clear(sf::Color::Black);
            if((debugMode ? printlayer[0] : true)) {
                Main::mainframe.frame.draw(*layer1);
            }
            if((debugMode ? printlayer[1] : true)) {
                Main::mainframe.frame.draw(*layer2);
            }
            actual->updateEvents(Main::player);
            for(Event *event : actual->getEvents()) {
                if(event->getPosition().y <= ppPosY) {
                    Main::mainframe.frame.draw(*event->getSprite());
                }
            }
            if(anim != Side::NO_MOVE && !anims) {
	      character.setTexture(Initializer::walkingPP[(int) anim]);
                animsCounter++;
                anims = animsCounter > 8;

            } else if(anim != Side::NO_MOVE && anims) {
	      character.setTexture(Initializer::walkingPP2[(int) anim]);
                animsCounter++;
                if(animsCounter > 16) {
                    anims = false;
                    animsCounter = 0;
                }
            } else if(anim == Side::NO_MOVE) {
	      character.setTexture(Initializer::texturePP[(int) ppDir]);
            }
            Main::mainframe.frame.draw(character);
            for(Event *event : actual->getEvents()) {
                if(event->getPosition().y > ppPosY) {
                    Main::mainframe.frame.draw(*event->getSprite());
                }
            }
            if((debugMode ? printlayer[2] : true)) {
                Main::mainframe.frame.draw(*layer3);
            }
            if(moving == Side::NO_MOVE) {
                std::vector<Event *> nextEvents = actual->getEvent(sf::Vector2i(ppPosX CASES, ppPosY CASES));
                if(nextEvents.size() > 0) {
                    for(Event *nextEvent : nextEvents) {
                        if(nextEvent->getEventTrigger() == Events::EventTrigger::BE_IN) {
                            bool go = false;
                            if(((nextEvent->getSide() & SIDE_UP) == SIDE_UP) && ppDir == Side::TO_UP) {
                                go = true;
                            } else if(((nextEvent->getSide() & SIDE_DOWN) == SIDE_DOWN) && ppDir == Side::TO_DOWN) {
                                go = true;
                            } else if(((nextEvent->getSide() & SIDE_RIGHT) == SIDE_RIGHT) && ppDir == Side::TO_RIGHT) {
                                go = true;
                            } else if(((nextEvent->getSide() & SIDE_LEFT) == SIDE_LEFT) && ppDir == Side::TO_LEFT) {
                                go = true;
                            }
                            if(go) {
                                nextEvent->action(Main::player);
                            }
                        }
                    }
                }
            }
            actual->updateElements(Main::mainframe.frame);
            if(scrolling && !debugMode) {
                camera.setCenter(character.getPosition().x + 16, character.getPosition().y + 16);
            }
            Main::mainframe.frame.setView(camera);
            if(debugMode) {
                Main::mainframe.frame.draw(debugText);
                Main::mainframe.frame.draw(fpsPrint);
            }
            Main::mainframe.frame.display();
            Main::mainframe.winRefresh();
            if(anim == Side::NO_MOVE) {
                if(isKeyPressed(sf::Keyboard::Return)) {
                    int lx = ppPosX;
                    int ly = ppPosY;
                    switch(ppDir) {
                    case Side::TO_UP:
                        ly--;
                        break;
                    case Side::TO_DOWN:
                        ly++;
                        break;
                    case Side::TO_LEFT:
                        lx--;
                        break;
                    case Side::TO_RIGHT:
                        lx++;
                        break;
                    default:
                        break;
                    }
                    vector<Event *> events = actual->getEvent(sf::Vector2i(lx CASES, ly CASES));
                    /*if(events.size() == 0){
                      events = actual->getEvent(sf::Vector2i(ppPosX CASES, ppPosY CASES));
                      }*/

                    if(events.size() > 0) {
                        for(unsigned int i = 0; i < events.size(); i++) {
                            if(events[i]->getEventTrigger() == Events::EventTrigger::PRESS) {
                                bool go = false;
                                if(((events[i]->getSide() & SIDE_UP) == SIDE_UP) && ppDir == Side::TO_UP) {
                                    go = true;
                                } else if(((events[i]->getSide() & SIDE_DOWN) == SIDE_DOWN) && ppDir == Side::TO_DOWN) {
                                    go = true;
                                } else if(((events[i]->getSide() & SIDE_RIGHT) == SIDE_RIGHT) && ppDir == Side::TO_RIGHT) {
                                    go = true;
                                } else if(((events[i]->getSide() & SIDE_LEFT) == SIDE_LEFT) && ppDir == Side::TO_LEFT) {
                                    go = true;
                                }
                                if(go) {
                                    events[i]->action(Main::player);
                                }
                            }
                        }
                    }
                }
            }
            if(anim == Side::TO_UP) {
                if(frames - startFrames >= 7) {
                    if(moving == Side::TO_UP) {
                        character.move(0, -4);
                    }
                    anim = Side::NO_MOVE;
                    moving = Side::NO_MOVE;
                } else {
                    if(moving == Side::TO_UP) {
                        character.move(0, -4);


                    }
                }
            }
            if(anim == Side::TO_DOWN) {
                if(frames - startFrames >= 7) {
                    if(moving == Side::TO_DOWN) {
                        character.move(0, 4);
                    }
                    anim = Side::NO_MOVE;
                    moving = Side::NO_MOVE;
                } else {
                    if(moving == Side::TO_DOWN) {
                        character.move(0, 4);
                    }
                }
            }

            if(anim == Side::TO_LEFT) {
                if(frames - startFrames >= 7) {
                    if(moving == Side::TO_LEFT) {
                        character.move(-4, 0);
                    }

                    anim = Side::NO_MOVE;
                    moving = Side::NO_MOVE;
                } else {
                    if(moving == Side::TO_LEFT) {
                        character.move(-4, 0);
                    }
                }
            }

            if(anim == Side::TO_RIGHT) {
                if(frames - startFrames >= 7) {
                    if(moving == Side::TO_RIGHT) {
                        character.move(4, 0);
                    }
                    anim = Side::NO_MOVE;
                    moving = Side::NO_MOVE;
                } else {
                    if(moving == Side::TO_RIGHT) {
                        character.move(4, 0);


                    }
                }
            }



        } else {
	  Utils::wait(FPS_TICKS - (GET_TICKS - ancientTick));
        }


    }
    return 0;
}

/**
 * Loop used when the player speak to a NPC
 */
int Overworld::boucleDialog(vector<sf::String> const& dialogs) {
      // `&dialogs[0]` converts the std::vector into a regular array.
    Dialog dialog(&dialogs[0], dialogs.size());


    sf::Vector2f posArrow = Main::mainframe.frame.mapPixelToCoords(sf::Vector2i(512-75, 512-30));
    Main::mainframe.arrDial.setPosition(posArrow);


    while(!dialog.isDialogOver()) {
        if((GET_TICKS - ancientTick >= FPS_TICKS)) {
            frames++;
            if(justTp) {
                tpCount++;
                justTp = tpCount < 0;
            }

            ancientTick = GET_TICKS;
            Main::mainframe.window.pollEvent(Main::mainframe.events);

            switch(Main::mainframe.events.type) {
                RETURN_ON_CLOSE_EVENT

            case sf::Event::KeyPressed:
                if(Main::mainframe.events.key.code == sf::Keyboard::Space) {
                    dialog.pass();
                }
                break;
            default:
                break;
            }
            if(isKeyPressed(sf::Keyboard::Escape)) {
                Main::player.gameIsOver = true;
                return -1;
            }
            Main::mainframe.frame.draw(*layer1);
            Main::mainframe.frame.draw(*layer2);
            for(Event *event : actual->getEvents()) {
                if(event->getPosition().y <= ppPosY) {
                    Main::mainframe.frame.draw(*event->getSprite());
                }
            }
            if(anim != Side::NO_MOVE && !anims) {
	      character.setTexture(Initializer::walkingPP[(int) anim]);
                animsCounter++;
            } else if(anim != Side::NO_MOVE && anims) {
	      character.setTexture(Initializer::walkingPP2[(int) anim]);
                animsCounter++;
            } else if(anim == Side::NO_MOVE) {
	      character.setTexture(Initializer::texturePP[(int) ppDir]);
            }
            Main::mainframe.frame.draw(character);
            for(Event *event : actual->getEvents()) {
                if(event->getPosition().y > ppPosY) {
                    Main::mainframe.frame.draw(*event->getSprite());
                }
            }
            Main::mainframe.frame.draw(*layer3);
            if(scrolling) {
                camera.setCenter(character.getPosition().x + 16, character.getPosition().y + 16);
            }
            Main::mainframe.frame.setView(Main::mainframe.frame.getDefaultView());
            Main::mainframe.frame.setView(camera);
            actual->updateElements(Main::mainframe.frame);

            dialog.updateTextAnimation();

            dialog.draw();
            Main::mainframe.frame.display();
            Main::mainframe.winRefresh();



        } else {
            Utils::wait(FPS_TICKS - (GET_TICKS - ancientTick));
        }


    }
    return 0;
}


