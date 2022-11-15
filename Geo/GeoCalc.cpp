//
// Created by dkulpa on 08.08.2022.
//

#include <iostream>
#include "GeoCalc.h"

double GeoCalc::calcHeading(const GeoPoint &p1, const GeoPoint &p2) {
    double gamma1 = (p1.getLat() * M_PI) / 180;
    double gamma2 = (p2.getLat() * M_PI) / 180;
    double ddelta = (p2.getLon() - p1.getLon()) * M_PI / 180;

    double a = cos(gamma2) * sin(ddelta);
    double b = cos(gamma1) * sin(gamma2) - sin(gamma1) * cos(gamma2) * cos(ddelta);

    double beta = atan2(a, b);
    beta = (beta * 180) / M_PI;

    if (beta < 0)
        beta = 360 + beta;

    return beta;
}

double GeoCalc::calcDistance(const GeoPoint &p1, const GeoPoint &p2) {
    double R = 6371e3; // Meters
    double gamma1 = p1.getLat() * M_PI/180;  // φ, λ in radians
    double gamma2 = p2.getLat() * M_PI/180;
    double dgamma = (p2.getLat()-p1.getLat()) * M_PI/180;
    double ddelta = (p2.getLon()-p1.getLon()) * M_PI/180;

    double a = sin(dgamma/2) * sin(dgamma/2) +
            cos(gamma1) * cos(gamma2) * sin(ddelta/2) * sin(ddelta/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    //std::cout << "Dist btwn: " << p1.getLat() << "/" << p1.getLon() << " and " << p2.getLat() << "/" <<
    //p2.getLon() << " is " << R*c << " m" << std::endl;

    return R * c;  // in metres
}

double GeoCalc::calcHeadingDiff(double h1, double h2) {
    return fmin((fabs(h1-h2)),(360- fabs(h1-h2)));
}

