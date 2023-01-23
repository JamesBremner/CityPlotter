#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "cxy.h"

class cRoad
{
public:
    cxy myEnd1;
    cxy myEnd2;

    /**
     * @brief Construct a new cRoad object
     * 
     * @param e1 
     * @param e2 
     * 
     * Road must be specified in approximately east to west or north to south
     * The CTOR will check for this and swap ends to conform
     */
    cRoad(const cxy &e1, const cxy &e2);
    
    void display();
};

class cIntersection
{
public:
    cRoad *myRoad1;
    cRoad *myRoad2;
    cxy myLocation;

    cIntersection()
        : myRoad1(0), myRoad2(0) {}

    cIntersection(
        cRoad &r1,
        cRoad &r2,
        const cxy &i)
        : myRoad1(&r1), myRoad2(&r2), myLocation(i)
    {
    }
    cRoad EastToWest();
    cRoad NorthToSouth();
    cRoad WestToEast();

    void display();

private:
    cRoad closest(const cxy &d1, const cxy &d2);
};

class cCity
{
public:
    std::vector<cRoad> myRoad;
    std::vector<cIntersection> myIntersection;

    void generate1();
    std::vector<std::vector<cxy>> plotfinder();
    void DisplayRoads();

private:
    bool nextIntersection(
        cIntersection &next,
        const cIntersection &inter,
        const cRoad &r);
};

cRoad::cRoad(const cxy &e1, const cxy &e2)
    : myEnd1(e1), myEnd2(e2)
{
    double a = cxy::angle(
        cxy(0, 0), cxy(1,0),
        e1, e2);

    // check road is specified east to west
    if( fabs(a) < 0.8)
        return;
    
    // check road is specified north to south
    if( fabs( a + 1.57 ) < 0.8)
        return;

    // swap road direction
    myEnd1 = e2;
    myEnd2 = e1;

    std::cout << a << "\n";
}

cRoad cIntersection::EastToWest()
{
    cxy we1(0, 0);
    cxy we2(10, 0);
    return closest(we1, we2);
}
cRoad cIntersection::NorthToSouth()
{
    cxy ns1(0, 10);
    cxy ns2(0, 0);
    return closest(ns1, ns2);
}
cRoad cIntersection::WestToEast()
{
    cxy ns1(10, 0);
    cxy ns2(0, 0);
    return closest(ns1, ns2);
}

cRoad cIntersection::closest(const cxy &d1, const cxy &d2)
{
    cRoad ret(myLocation, myRoad1->myEnd2);

    double ra = fabs(cxy::angle(d1, d2,
                                myRoad1->myEnd1, myRoad1->myEnd2));

    double a = fabs(cxy::angle(d1, d2,
                               myRoad2->myEnd1, myRoad2->myEnd2));
    if (a < ra)
    {
        ra = a;
        ret.myEnd2 = myRoad2->myEnd2;
    }

    a = fabs(cxy::angle(d1, d2,
                        myRoad1->myEnd2, myRoad1->myEnd1));
    if (a < ra)
    {
        ra = a;
        ret.myEnd2 = myRoad1->myEnd1;
    }

    a = fabs(cxy::angle(d1, d2,
                        myRoad2->myEnd2, myRoad2->myEnd1));
    if (a < ra)
    {
        ra = a;
        ret.myEnd2 = myRoad2->myEnd1;
    }

    // std::cout << rn << " " << ra << "\n";

    return ret;
}

void cIntersection::display()
{
    std::cout << "intersection at "
              << myLocation.x << " " << myLocation.y << "\n";
}

void cCity::generate1()
{
    myRoad.push_back(cRoad(cxy(100, 0), cxy(-100, 0)));
    myRoad.push_back(cRoad(cxy(-100, 10), cxy(100, 10)));
    myRoad.push_back(cRoad(cxy(0, 100), cxy(0, -100)));
    myRoad.push_back(cRoad(cxy(10, 100), cxy(10, -100)));

    DisplayRoads();

    for (int kr1 = 0; kr1 < myRoad.size(); kr1++)
        for (int kr2 = kr1 + 1; kr2 < myRoad.size(); kr2++)
        {
            cxy p;
            if (cxy::isIntersection(
                    p,
                    myRoad[kr1].myEnd1, myRoad[kr1].myEnd2,
                    myRoad[kr2].myEnd1, myRoad[kr2].myEnd2))
            {
                myIntersection.push_back(
                    cIntersection(myRoad[kr1], myRoad[kr2], p));
                // std::cout
                //     << kr1 << " " << kr2 << " at " << p.x << " " << p.y << "\n";
            }
        }
}

void cCity::DisplayRoads()
{
    for (auto &r : myRoad)
        r.display();
}
void cRoad::display()
{
    std::cout << "road from " << myEnd1.x << ", " << myEnd1.y
              << " to " << myEnd2.x << ", " << myEnd2.y << "\n";
}

bool cCity::nextIntersection(
    cIntersection &next,
    const cIntersection &inter,
    const cRoad &r)
{
    for (auto &nextInter : myIntersection)
    {
        if (inter.myLocation.x == nextInter.myLocation.x &&
            inter.myLocation.y == nextInter.myLocation.y)
            continue;
        double d = nextInter.myLocation.dis2toline(r.myEnd1, r.myEnd2);
        if (d < 1)
        {
            // std::cout << "next " << d << " ";
            // nextInter.display();
            next = nextInter;
            return true;
        }
    }
    // no more intersections
    return false;
}

std::vector<std::vector<cxy>> cCity::plotfinder()
{
    // vector of plots
    std::vector<std::vector<cxy>> vplot;

    // loop over intersections
    for (auto &inter : myIntersection)
    {
        // check if this is the north east corner of a quadrilateral plot

        std::vector<cxy> plot;
        plot.push_back(inter.myLocation);

        // find road from intersection going in approx east west direction
        auto road = inter.EastToWest();

        // find next intersection on road
        cIntersection next1;
        if (!nextIntersection(next1, inter, road))
            continue; // intersection does not exist, so this is not a plot

        // add to plot
        plot.push_back(next1.myLocation);

        // continue to check the remaining two plot edges, if they exist

        road = next1.NorthToSouth();
        cIntersection next2;
        if (!nextIntersection(next2, next1, road))
            continue;
        plot.push_back(next2.myLocation);

        road = next2.WestToEast();
        cIntersection next3;
        if (!nextIntersection(next3, next2, road))
            continue;
        plot.push_back(next3.myLocation);

        // found a good plot, add to list
        vplot.push_back(plot);
    }

    return vplot;
}

main()
{
    cCity theCity;

    theCity.generate1();

    auto vplot = theCity.plotfinder();

    std::cout << vplot.size() << " plots found\n";
    for (auto &p : vplot)
    {
        for (auto &c : p)
        {
            std::cout << c.x << " " << c.y << ", ";
        }
        std::cout << "\n";
    }

    return 0;
}
