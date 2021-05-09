// special-hw-1-4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "cg-engine/cg-engine.h"

// C Libraries
#include <cstdint>
#include <cmath>
#include <cstdlib>

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <queue>
#include <thread>
#include <chrono>
#include <map>
#include <stack>
#include <set>
#include <tuple>
#include <memory>
#include <iomanip>

//using namespace std;

namespace mainNS {

using namespace cgEngine;
using std::cout;
using std::endl;

class ThisGame: public Game {
  public:
    struct Score {
      int player = 0;
      int computer = 0;
    } score;
    ThisGame(IScreen& screen, Input& input): Game(screen, input) {}
};

struct Ball : public RectangleObject {
    Screen& screen;
    int direction = 1;
    
    Ball(Screen& screen): screen{screen}, RectangleObject{{0, 0}, {2, 0}, {2, 2}, {0, 2}} {
        static EventHandler callback = [this](IView* obj, const Message& message, const MessageData* data) { collideHandler(obj, Messages::collide()); };
        static EventHandler nearTheBorderCallback = [this](IView* obj, const Message& message, const MessageData* pData) {
            decltype(auto) msgData = const_cast<MessageData&>(*pData).getSuccessor<BordersMessageData>();
            nearTheBorderHandler(obj, Messages::nearTheBorder(), msgData);  
            };
        addEventHandler(&callback);
		addHandler(Messages::nearTheBorder(), &nearTheBorderCallback);
        setMinMaxByScreen();
        setMoveVector({1, 1});
        setSpeed(50);
        setRandomPosition();
    }

    void setRandomPosition() {
        float randomX = static_cast<short>(rand() % 30 + 10);
		float randomY = static_cast<short>(rand() % 30 + 10);
        setPosition({randomX, randomY});
    }
    Coords createRepusliveVector() {
		Coords newBasisI{ 0, 1 };
		Coords newBasisJ{ -1, 0 };
		return getMoveVector().applyNewBasis(newBasisI, newBasisJ);
    }
    void collideHandler(IView* obj, const Message& message) {
        if (message == Messages::collide()) {
            setMoveVector(createRepusliveVector());
            bEnableCollision = false;
			ThisGame::game->setTimer(25, [this]() {
                bEnableCollision = true;
				});
        }	
    }

    void nearTheBorderHandler(IView* obj, const Message& message, const BordersMessageData& bordersData) {
        /*cout << "\n\n nearTheBorder pos: " << obj->getPosition().x << ", " << obj->getPosition().y << endl;*/
        bool bLeft = bordersData.bLeft;
        bool bRight = bordersData.bRight;
        bool bTop = bordersData.bTop;
        bool bBottom = bordersData.bBottom;
        //auto timer = ThisGame::game->setTimer(1000, [this]() {
        //    Coords moveVec = getMoveVector();
        //    setMoveVector({moveVec.x * -1, moveVec.y * -1});
        //    });
        //timer->bRepeat = true;
		if (bLeft && bTop) {
			setMoveVector({ 1, 1 });
		}
		else if (bRight && bBottom) {
			setMoveVector({ -1, -1 });
        }
        else {
            setMoveVector(createRepusliveVector(/*obj, getMoveVector()*/));
        }

        // Score handling
        auto& score = static_cast<ThisGame*>(ThisGame::game)->score;
        if (bLeft) {
            score.computer++;
        }
        if (bRight) {
            score.player++;
            
        }
        cout << "\n-------------------------- [Score] Player: " << score.player << " Computer: " << score.computer << endl;
        if (score.player > 100 || score.computer > 100) {
            clearConsole();
            if (score.player > score.computer) {
                cout << "============ Player WIN! ============" << endl;
            }
            else if (score.computer > score.player) {
                cout << "============ Computer WIN! ============" << endl;
            }
            else {
                cout << "============ DRAW ============" << endl;
            }
            static_cast<ThisGame*>(ThisGame::game)->bStop = true;
            Application::pause();
        }
    }

   /* vector<unsigned long long> times;*/
    //vector<float> moves;
    //int maxCount = 50;
	void TickHandler(unsigned long long deltaTime) override {
        ViewObject::TickHandler(deltaTime);
  //      static int counter = 0;
  //      /*if (direction == 0) return;*/
  //      //deltaTime = 30; //0,030 * 100 = 3
  //      float moveDist = (deltaTime / 1000.0) * speed;

  ////      auto pos = this->getPosition();
		////if (pos.y >= screen.height - 3 && direction == 1)
		////	return;
		////if (pos.y <= 0 && direction == 2)
		////	return;

  //      /*if (counter++ == 5) ++direction;*/
  //      /*moves.push_back(moveDist);*/
  //      static bool b = false;
  //      //if (direction == 1) {
  //      //    /*move(screen, moveDist, moveDist);*/
  //      //    setMoveVector({1, 1});
  //      //}
  //      //    
  //      //else if (direction == 2) {
  //      //    /*move(screen, -moveDist, -moveDist);*/
  //      //    if (!b) {
  //      //        setMoveVector({ -1, -1 });
  //      //        b = true;
  //      //    }
  //      //    
  //      //}
		////else if (direction == 5)
		////	move(screen, -moveDist, moveDist);
  //      ViewObject::TickHandler(deltaTime);
	}
};

void run() {
	Screen screen(50, 50);
	Input input;
	shared_ptr<ViewObject> pPlayer;
	function<void(const vector<char>&, map<char, bool>)> handler =
		[&pPlayer, &screen](const vector<char>& chars, map<char, bool> states) {
		bool isPressed = false;
		char chPressed = -1;
		for (auto pair : states) {
			if (pair.second) {
				isPressed = true;
				chPressed = pair.first;
				break;
			}
		}
		if (!isPressed) {
			pPlayer->setDirection(0);
			/*cout << "dir, ch: " << (int)pPlayer->getDirection() << ", " << chPressed << endl;*/
			return;
		}
		static map<char, uint8_t> directions = {
			{'d', 3},
			{'a', 7},
			{'w', 1},
			{'s', 5},
		};
		auto itDir = directions.find(chPressed);
		if (chPressed == 'd') {
			/*cout << "test";*/
		}
		if (itDir != directions.end()) {
			pPlayer->setDirection(itDir->second);
		}
		/*cout << "dir, ch: " << (int)pPlayer->getDirection() << ", " << itDir->first << endl;*/
	};
	input.add({ 'a', 'd', 'w', 's' }, handler);
	ThisGame game{ screen, input };
	//screen.set(3, 4, true);
	//screen.set(4, 4, true);
	//screen.set(3, 5, true);
	//screen.set(4, 5, true);

	//Line line{Point{1, 7}, Point{46, 49}, true};
	//line.render(screen);
	////Screen screen(10, 10);
	//Line line2{ Point{44, 4}, Point{44, 9}, true };
	//line2.render(screen);

	//Rectangle rect{Point{10, 10}, Point{30, 10}, Point{30, 20}, Point{10, 20}, true};
	//rect.render(screen);

	//Point pt{2, 2};
	//pt.render(screen);

	screen.draw();
	////////////////////
	auto fnDirToMove = [](char dir, float move) {
		static map<int, array<int, 2>> mapDirMove = {
			{0, {0, 0}},
			{1, {0, -1}},
			{2, {1, -1}},
			{3, {1, 0}},
			{4, {1, 1}},
			{5, {0, 1}},
			{6, {-1, 1}},
			{7, {-1, 0}},
			{8, {-1, -1}}
		};
		auto& factors = mapDirMove[dir];
		return make_tuple(factors[0] * move, factors[1] * move);
	};

	////////// Bot /////////////
	auto pBot = Builder::createLineObject(shapes::Point{ 46, 0 }, shapes::Point{ 46, 19 });
	pBot->setMinMaxByScreen();
	pBot->setSpeed(100);
	auto timer = ThisGame::game->setTimer(200, [&pBot]() {
		auto nRandom = rand();
		Coords moveVector = { 0, static_cast<float>(1 * (nRandom % 2) ? 1 : -1) };
		pBot->setMoveVector(moveVector);
		});
	timer->bRepeat = true;
	////////////////////////////

	pPlayer = Builder::createLineObject(shapes::Point{ 0, 0 }, shapes::Point{ 0, 9 });
	static EventHandler nearTheBorderCallback = [](IView* obj, const Message& message, const MessageData* pData) {
		Coords pos = obj->getPosition();
		//cout.setf(ios::fixed);
		//cout << "=================================[LineObject] message: " << message
		//	<< setprecision(2) << " pos: " << pos.x << ", " << pos.y << endl;
	};
	pPlayer->addEventHandler(&nearTheBorderCallback);
	pPlayer->setMinMaxByScreen();
	pPlayer->setSpeed(50);
	/*pPlayer->direction = 4;*/
	//uint8_t direction = 0;
	/*cout << "\npPlayer1: " << pPlayer.get() << "dir: " << (int)pPlayer->direction << endl;*/
	pPlayer->fnTickHandler = [&pPlayer, &screen, &fnDirToMove](unsigned long long deltaTime) {
		pPlayer->ViewObject::TickHandler(deltaTime);

		static decltype(screen.width) gap = 1;
		Coords curPos = pPlayer->getPosition();
		//if (coords.x <= 0) {
		//    pPlayer->setPosition({0, coords.y});
		//    return;
		//}


		/*deltaTime = 30;*/
		float moveDist = (pPlayer->speed / 1000.0f) * deltaTime;

		if (pPlayer->getDirection() != 0) {
			if (pPlayer->getDirection() == 1) {
				pPlayer->move(screen, 0, -moveDist);
			}
			else if (pPlayer->getDirection() == 3) {
				pPlayer->move(screen, moveDist, 0);
			}
			else if (pPlayer->getDirection() == 5) {
				pPlayer->move(screen, 0, moveDist);
			}
			else if (pPlayer->getDirection() == 7) {
				pPlayer->move(screen, -moveDist, 0);
			}
		}
	};

	auto ball = make_shared<Ball>(screen);
	game.add(pPlayer);
	game.add(ball);
	game.add(pBot);
	game.run();
}

}

int main()
{
 //   PointEquation pt1{1, 2};
 //   PointEquation pt2{1, 2};
 //   PointCollision pc1{pt1};
 //   PointCollision pc2{pt2};
 //   cout << "test1: " << pc1.isCollide(pc2) << endl;

 //   LineEquation lnEq1, lnEq2;
 //   lnEq1.init(pt1, pt2);
 //   lnEq2.init(pt1, pt2);
 //   LineCollision lc1{lnEq1}, lc2{lnEq2};
 //   cout << "test2: " << lc1.isCollide(lc2) << endl;

 //   cout << "test3: " << lc1.isCollide(pc1) << endl;
 //   cout << "test4: " << pc1.isCollide(lc1) << endl;

 //   cout << "test5: " << LineCollision{ {2, 2}, {4, 4} }.isCollide(PointCollision{3, 3}) << endl;
 //   cout << "test6: " << LineCollision{ {2, 2}, {4, 4} }.isCollide(PointCollision{ 2, 2 }) << endl;
 //   cout << "test7: " << LineCollision{ {2, 2}, {4, 4} }.isCollide(PointCollision{ 4, 4 }) << endl;

 //   cout << "test8: " << !LineCollision{ {2, 2}, {4, 4} }.isCollide(PointCollision{ 3, 2 }) << endl;
 //   cout << "test9: " << !LineCollision{ {2, 2}, {4, 4} }.isCollide(PointCollision{ 1, 1 }) << endl;

 //   cout << "test10: " << !LineCollision{ {2, 2}, {4, 4} }.isCollide(LineCollision{ {2, 1}, {4, 3} }) << endl;
 //   cout << "test11: " << !LineCollision{ {2, 2}, {4, 4} }.isCollide(LineCollision{ {0, 0}, {1, 1} }) << endl;

 //   cout << "test12: " << LineCollision{ {2, 2}, {4, 4} }.isCollide(LineCollision{ {0, 0}, {2, 2} }) << endl;
 //   cout << "test13: " << LineCollision{ {2, 2}, {4, 4} }.isCollide(LineCollision{ {0, 0}, {3, 3} }) << endl;
 //   cout << "test14: " << LineCollision{ {2, 2}, {4, 4} }.isCollide(LineCollision{ {4, 2}, {2, 4} }) << endl;

 //   cout << "test15: " << TriangleCollision{ {2, 2}, {5, 5}, {5, 2} }.isCollide(PointCollision{ {2, 2} }) << endl;
 //   cout << "test16: " << !TriangleCollision{ {2, 2}, {5, 5}, {5, 2} }.isCollide(PointCollision{ {2, 4} }) << endl;

 //   cout << "test17: " << TriangleCollision{ {2, 2}, {5, 5}, {5, 2} }.isCollide(LineCollision{ {2, 1}, {4, 3} }) << endl;
 //   cout << "test18: " << !TriangleCollision{ {2, 2}, {5, 5}, {5, 2} }.isCollide(LineCollision{ {2, 1}, {1, 2} }) << endl;

 //   cout << "test19: " << TriangleCollision{ {2, 2}, {5, 5}, {5, 2} }
 //       .isCollide(TriangleCollision{ {3, 1}, {4, 3}, {4, 1} }) << endl;
 //   cout << "test20: " << !TriangleCollision{ {2, 2}, {5, 5}, {5, 2} }
 //       .isCollide(TriangleCollision{ {1, 3}, {3, 6}, {3, 4} }) << endl;

	//cout << "test21: " << PolylineCollision{ {{2, 2}, {5, 5}, {5, 2}, {2, 2}} }
	//	.isCollide(PolylineCollision{ {{3, 1}, {4, 3}, {4, 1}, {3, 1} }}) << endl;
	//cout << "test22: " << !PolylineCollision{ {{2, 2}, {5, 5}, {5, 2}, {2, 2}} }
	//	.isCollide(PolylineCollision{ {{1, 3}, {3, 6}, {3, 4}, {1, 3} }}) << endl;
 //   return 0;
 ////////////////////////////////////////////////

	mainNS::run();
}
