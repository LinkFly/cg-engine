// special-hw-1-4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Screen.h"
#include "Shapes.h"
#include "Objects.h"
#include "Game.h"

#include "inequalities.h"

#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <cmath>
#include <functional>
#include <queue>
#include <thread>
#include <chrono>
#include <map>
#include <stack>
#include <set>
#include <tuple>

using namespace std;

bool testGetCross() {
    LineInequality line1({ 2, 2 }, { 4, 4 });
    LineInequality line2({ 2, 4 }, { 4, 2 });
    Coords cross1;
    bool res = line1.getCross(line2, cross1);
    cout << "cross1: x = " << cross1.x << ", y = " << cross1.y << endl;
    return res;
}

struct Ball : public RectangleObject {
    Screen& screen;
    int direction = 3;
    Ball(Screen& screen): screen{screen}, RectangleObject{{0, 0}, {2, 0}, {2, 2}, {0, 2}} {
        speed = 1;
    }
   /* vector<unsigned long long> times;*/
    vector<float> moves;
    int maxCount = 50;
	void TickHandler(unsigned long long deltaTime) override {
        //times.push_back(deltaTime);
        //if (maxCount-- == 0) {
        //    cout << times[10] << endl;
        //}
        static int counter = 0;
        //deltaTime = 30; //0,030 * 100 = 3
        float moveDist = (deltaTime / 1000.0) * speed;
        auto pos = this->getPosition();
        if (pos.y >= screen.height - 3)
            return;
        if (counter++ == 5) ++direction;
        /*moves.push_back(moveDist);*/
        if (true) //direction == 3)
            move(screen, moveDist, moveDist);
        else if (direction == 4)
            move(screen, 0, moveDist);
		else if (direction == 5)
			move(screen, -moveDist, moveDist);
	}

    void message(IView* object, string message) override {
        static int collided_count = 0;
        if (message == Messages::collide())
            cout << "\n!!!!!!!!! COLLIDED Number: " << ++collided_count << endl;
	}
};

int main()
{
    /*cout << "Test getCross: " << testGetCross() << endl;
    exit(0);*/
    //std::cout << '\xDB' << '\xDC' << '\xDF' << endl;
    Screen screen(50, 50);
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

    auto myObj = Builder::createLineObject(shapes::Point{ 10, 20 }, shapes::Point{ 10, 40 });
    myObj->speed = 1;
    /*myObj->direction = 4;*/
    //uint8_t direction = 0;
    /*cout << "\nmyObj1: " << myObj.get() << "dir: " << (int)myObj->direction << endl;*/
    myObj->fnTickHandler = [&myObj, &screen, &fnDirToMove](unsigned long long deltaTime) {
        //cout << "dir: " << (int)myObj->direction << endl;
        deltaTime = 30;
		float moveDist = myObj->speed * deltaTime / 1000.0;
        //cout << " - -- -- " << moveDist;
        //myObj->move(screen, moveDist, 0);
        //return;
        /*cout << "\n\n\nmyObj2: " << myObj.get() << "dir: " << (int)myObj->direction << endl;*/
        if (myObj->getDirection() != 0) {
            //if (myObj->getDirection() != 1) {
            //    cout << myObj->getDirection();
            //}
            //cout << "++++++++++   " << (int)myObj->getDirection() << "  ";
            //auto dists = fnDirToMove(myObj->getDirection(), moveDist);
            //cout << "\n                                " << (float)(get<0>(dists)) << ",           " << (float)(get<1>(dists));
            /*myObj->move(screen, get<0>(dists), get<1>(dists));*/
			if (myObj->getDirection() == 1) {
				myObj->move(screen, 0, -moveDist);
			}
			else if (myObj->getDirection() == 3) {
				myObj->move(screen, moveDist, 0);
			}
            if (myObj->getDirection() == 5) {
                myObj->move(screen, 0, moveDist);
            }
			//else if (myObj->getDirection() == 4) {
			//	myObj->move(screen, moveDist, moveDist);
			//}
			else if (myObj->getDirection() == 7) {   
				myObj->move(screen, -moveDist, 0);
			}
            /*myObj->render(screen);*/
            //auto pScrBuffer = (screen.addGetBuffer());
            //myObj->render(*pScrBuffer);
        }
    };

    Input input;
	function<void(const vector<char>&, map<char, bool>)> handler = 
        [&myObj, &screen](const vector<char>& chars, map<char, bool> states) {
            bool isPressed = false;
            char chPressed = -1;
            for(auto pair : states) {
                if (pair.second) {
                    isPressed = true;
                    chPressed = pair.first;
                    break;
                }
            }
            if (!isPressed) {
                myObj->setDirection(0);
                cout << "dir, ch: " << (int)myObj->getDirection() << ", " << chPressed << endl;
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
                myObj->setDirection(itDir->second);
            }
            cout << "dir, ch: " << (int)myObj->getDirection() << ", " << itDir->first << endl;
	    };
    input.add({'a', 'd', 'w', 's'}, handler);
	/* input.add('d', handler);
	 input.add('w', handler);
	 input.add('s', handler);*/
    Game game{screen, input};

    /*shared_ptr<IShape> newLine = make_shared<Line>(Point{10, 10}, Point{20, 20});
    shared_ptr<ShapeCollection> pShapes = make_shared<ShapeCollection>();
    pShapes->push_back(newLine);
    auto myObj = make_shared<ViewObject>();
    myObj->setShapes(pShapes);*/
    
    auto ball = make_shared<Ball>(screen);
    game.tree->getChildren()->add(myObj);
    game.tree->getChildren()->add(ball);
    game.run();
}
