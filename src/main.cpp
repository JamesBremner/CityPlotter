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

    cRoad() {}

    void display();

    bool operator==(const cRoad &other) const;

    cRoad reverse();

    double angle( const cRoad& other ) const;
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
    cRoad EastToWest() const;
    cRoad NorthToSouth() const;
    cRoad WestToEast() const;

    void display() const;

private:
    cRoad closest(const cxy &d1, const cxy &d2) const;
};

class cCity
{
public:
    std::vector<cRoad> myRoad;
    std::vector<cIntersection> myIntersection;

    void generate1();
    void generate2();

    /// @brief Find plots encircled by roads
    /// @return vector of plots
    std::vector<std::vector<cxy>> plotfinder();

    void displayRoads();
    void displayIntersections();

private:
    void constructIntersections();

    /**
     * @brief Find next intersection in circuit of possible plot
     *
     * @param[out] next the next intersection, if it exists
     * @param inter the current intersection
     * @param r the road used to reach this intersection
     * @return true intersection found
     * @return false no intersection
     *
     * This "turns right" at the intesection
     */

    const cIntersection *nextIntersection(
        const cRoad &road);

    cRoad nextRoad(
        const cRoad &inRoad,
        const cIntersection &inter);
};

cRoad::cRoad(const cxy &e1, const cxy &e2)
    : myEnd1(e1), myEnd2(e2)
{
    double a = cxy::angle(
        cxy(0, 0), cxy(1, 0),
        e1, e2);

    // check road is specified east to west
    if (fabs(a) < 0.8)
        return;

    // check road is specified north to south
    if (fabs(a + 1.57) < 0.8)
        return;

    // swap road direction
    myEnd1 = e2;
    myEnd2 = e1;

    std::cout << a << "\n";
}

cRoad cIntersection::EastToWest() const
{
    cxy we1(0, 0);
    cxy we2(10, 0);
    return closest(we1, we2);
}
cRoad cIntersection::NorthToSouth() const
{
    cxy ns1(0, 10);
    cxy ns2(0, 0);
    return closest(ns1, ns2);
}
cRoad cIntersection::WestToEast() const
{
    cxy ns1(10, 0);
    cxy ns2(0, 0);
    return closest(ns1, ns2);
}

cRoad cIntersection::closest(const cxy &d1, const cxy &d2) const
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

void cIntersection::display() const
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

    displayRoads();
    constructIntersections();
}

void cCity::generate2()
{
    myRoad.push_back(cRoad(cxy(100, 0), cxy(-100, 0)));
    myRoad.push_back(cRoad(cxy(0, 100), cxy(0, -100)));
    myRoad.push_back(cRoad(cxy(10, 100), cxy(10, -100)));
    myRoad.push_back(cRoad(cxy(-15, -5), cxy(5, 15)));
    myRoad.push_back(cRoad(cxy(5, 15), cxy(20, 0)));

    displayRoads();
    constructIntersections();
}

void cCity::constructIntersections()
{
    myIntersection.clear();
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

                //     std::cout << "\n";
                // myRoad[kr1].display();
                // myRoad[kr2].display();
                // std::cout
                //     << kr1 << " " << kr2 << " at " << p.x << " " << p.y << "\n";
            }
        }
}

void cCity::displayRoads()
{
    for (auto &r : myRoad)
        r.display();
}

void cCity::displayIntersections()
{
    for( auto& i : myIntersection )
        i.display();
}
void cRoad::display()
{
    std::cout << "road from " << myEnd1.x << ", " << myEnd1.y
              << " to " << myEnd2.x << ", " << myEnd2.y << "\n";
}

double cRoad::angle( const cRoad& other ) const
{
    return cxy::angle(
        myEnd1, myEnd2,
        other.myEnd1, other.myEnd2 );
}

bool cRoad::operator==(const cRoad &other) const
{
    return cxy::angle(myEnd1, myEnd2, other.myEnd1, other.myEnd2) < 0.1;
}

cRoad cRoad::reverse()
{
    cRoad ret;
    ret.myEnd1 = myEnd2;
    ret.myEnd2 = myEnd1;
    return ret;
}

const cIntersection *cCity::nextIntersection(const cRoad &road)
{
    const cIntersection *ret = 0;

    // check for road end
    if( road.myEnd1.dist2( road.myEnd2) < 1 )
        return ret;

    double dmin = INT_MAX;
    for (const auto &testInter : myIntersection)
    {
        // ignore intersection at start of road
        if (testInter.myLocation == road.myEnd1)
            continue;

        // ignore intersection not on road
        double d = testInter.myLocation.dis2toline(road.myEnd1, road.myEnd2);
        if (d > 1)
            continue;

        // return intersection closest to start of road
        d = road.myEnd1.dist2(testInter.myLocation);
        if (d < dmin)
        {
            dmin = d;
            ret = &testInter;
        }
    }
    return ret;
}

cRoad cCity::nextRoad(
    const cRoad &inRoad,
    const cIntersection &inter)
{
    // find road from intersection
    // with minimum anti-clockwise angle to incoming road
    cRoad ret;
    ret.myEnd1 = inter.myLocation;

    double a = 100;

    cRoad testRoad( *inter.myRoad1 );
    double at = inRoad.angle( testRoad );
    if( fabs( at ) > 0.1 ) {
        if (0 > at && at > -3.1) {
            a = at;
            ret.myEnd2 = testRoad.myEnd2;
        }
    }

    testRoad = testRoad.reverse();
    at = inRoad.angle( testRoad );
    if( fabs( at ) > 0.1 ) {
        if (0 > at && at > -3.1) {
            a = at;
            ret.myEnd2 = testRoad.myEnd2;
        }
    }

    testRoad = *inter.myRoad2;
    at = inRoad.angle( testRoad );
    if( fabs( at ) > 0.1 ) {
        if (0 > at && at > -3.1) {
            a = at;
            ret.myEnd2 = testRoad.myEnd2;
        }
    }

    testRoad = testRoad.reverse();
    at = inRoad.angle( testRoad );
    if( fabs( at ) > 0.1 ) {
        if (0 > at && at > -3.1) {
            a = at;
            ret.myEnd2 = testRoad.myEnd2;
        }
    }

    return ret;

}

std::vector<std::vector<cxy>> cCity::plotfinder()
{
    // vector of plots
    std::vector<std::vector<cxy>> vplot;

    // loop over intersections
    for (const auto &interStart : myIntersection)
    {
        std::vector<cxy> plot;
        plot.push_back(interStart.myLocation);

        // find road from intersection going in approx east west direction
        auto road = interStart.EastToWest();

        while (1)
        {

            //std::cout << "\n";
            //road.display();
            //std::cout << "leaving from ";
            //inter.display();

            // find next intersection on road
            const cIntersection *nextInter = nextIntersection(road);
            if (!nextInter)
            {
                // intersection does not exist, so this is not a plot
                plot.clear();
                break;
            }

            // check for plot completed
            if( interStart.myLocation == nextInter->myLocation )
                break;

            // add to plot
            plot.push_back( nextInter->myLocation );

            // select road from intersection
            road = nextRoad( road, *nextInter );
        }

        // check that a valid plot was found
        if( plot.size() )
            vplot.push_back( plot );
    }

    return vplot;
}

main()
{
    cCity theCity;

    theCity.generate2();

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
