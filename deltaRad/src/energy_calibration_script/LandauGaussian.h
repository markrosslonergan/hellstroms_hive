/**
 *  * Convoluted Landau and Gaussian Fitting function
 *   * Taken from https://root.cern.ch/root/html/tutorials/fit/langaus.C.html
 *    */

#ifndef LANDAUGAUSSIAN_H
#define LANDAUGAUSSIAN_H

#include "TMath.h"
#include <algorithm>

inline Double_t landauGaussian(Double_t *x, Double_t *par){
    //Fit parameters:
    //par[0]=Width (scale) parameter of Landau density
    //par[1]=Most Probable (MP, location) parameter of Landau density
    //par[2]=Total area (integral -inf to inf, normalization constant)
    //par[3]=Width (sigma) of convoluted Gaussian function
    //
    //In the Landau distribution (represented by the CERNLIB approximation),
    //the maximum is located at x=-0.22278298 with the location parameter=0.
    //This shift is corrected within this function, so that the actual
    //maximum is identical to the MP parameter.

    // Numeric constants
    Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
    Double_t mpshift  = -0.22278298;       // Landau maximum location
     
    // Double_t np = 100.0;      // number of convolution steps
    // Double_t sc =   100.0;      // convolution extends to +-sc Gaussian sigmas
    Double_t np = 1000.0;
    Double_t sc = par[1]/par[3];
    //

    // Variables
    Double_t xx;
    Double_t mpc;
    Double_t fland;
    Double_t sum = 0.0;
    Double_t xlow,xupp;
    Double_t step;
    Double_t i;

    // MP shift correction
    mpc = par[1] - mpshift * par[0];

    // Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];

    step = (xupp-xlow) / np;

    // Convolution integral of Landau and Gaussian by sum
    for(i=1.0; i<=np/2; i++) {
        xx = xlow + (i-.5) * step;
        Double_t fland_tmp = TMath::Landau(xx,mpc,par[0]) / par[0];
        fland = std::max(fland_tmp,std::numeric_limits<double>::min());
        sum += fland * TMath::Gaus(x[0],xx,par[3]);

        xx = xupp - (i-.5) * step;
        fland_tmp = TMath::Landau(xx,mpc,par[0]) / par[0];
        fland = std::max(fland_tmp,std::numeric_limits<double>::min());
        sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }

    double returner = par[2] * step * sum * invsq2pi / par[3];
    return std::max(returner, std::numeric_limits<double>::min());
}

#endif
