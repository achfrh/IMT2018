//
//  main.cpp
//  Quant lib
//


#include "binomialtree.hpp"
#include "binomialengine.hpp"
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/stochasticprocess.hpp>


#include <boost/timer.hpp>
#include <ql/time/calendars/target.hpp>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

using namespace QuantLib;

int main() {

    try {
        
        //Preparing Calendar
        Calendar calendar = TARGET();
        Date todaysDate(2, January, 2017);
        Settings::instance().evaluationDate() = todaysDate;
        DayCounter dayCounter = Actual365Fixed();

        // First example/First Option
        
        Option::Type type(Option::Call);
        Real strike = 50;
        Real underlying = 50;
        Spread dividendYield = 0.00;
        Rate riskFreeRate = 0.02;
        Volatility volatility = 0.15;
        Date maturity(5, February, 2018);
        Date settlementDate=maturity+2;
        std::cout << "Option type = "                   << type << std::endl;
        std::cout << "Maturity = "                      << maturity << std::endl;
        std::cout << "settlementDate = "                << settlementDate << std::endl;
        std::cout << "Strike = "                        << strike << std::endl;
        std::cout << "Underlying price = "              << underlying << std::endl;
        std::cout << "Risk-free interest rate = "       << riskFreeRate <<std::endl;
        std::cout << "Dividend yield = "                << dividendYield << std::endl;
        std::cout << "Volatility = "                    << volatility <<std::endl;
        std::cout << std::endl;
        std::string method;
        std::cout << std::endl ;
        
        Size timeSteps=100;
        
        Handle<Quote> underlying_(boost::shared_ptr<Quote>(new SimpleQuote(underlying)));
        
        boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity)); // European exercise

        Handle<YieldTermStructure> flatTermStructure(boost::shared_ptr<YieldTermStructure>(
            new FlatForward(maturity, riskFreeRate, dayCounter)));
        
        Handle<YieldTermStructure> flatDividendTermStructure(boost::shared_ptr<YieldTermStructure>(
            new FlatForward(maturity, dividendYield, dayCounter)));
        
        Handle<BlackVolTermStructure> flatVolTermStructure(boost::shared_ptr<BlackVolTermStructure>(
            new BlackConstantVol(maturity, calendar, volatility,dayCounter)));          //Vol
        
        boost::shared_ptr<StrikedTypePayoff> payoff(
            new PlainVanillaPayoff(type, strike));                                  //Payoff
        
        boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
            new BlackScholesMertonProcess(underlying_, flatDividendTermStructure,flatTermStructure, flatVolTermStructure));
        
        VanillaOption europeanOption(payoff, europeanExercise);
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<JarrowRudd>(bsmProcess, timeSteps)));
        std::cout << "Delta equals"  << europeanOption.delta() << std::endl;
        std::cout << "Gamma equals"  << europeanOption.gamma() << std::endl;
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<CoxRossRubinstein>(bsmProcess, timeSteps)));
        std::cout << "Delta equals"  << europeanOption.delta() << std::endl;
        std::cout << "Gamma equals"  << europeanOption.gamma() << std::endl;
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Trigeorgis>(bsmProcess, timeSteps)));
        std::cout << "Delta equals"  << europeanOption.delta() << std::endl;
        std::cout << "Gamma equals"  << europeanOption.gamma() << std::endl;
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Tian>(bsmProcess, timeSteps)));
        std::cout << "Delta equals"  << europeanOption.delta() << std::endl;
        std::cout << "Gamma equals"  << europeanOption.gamma() << std::endl;
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<LeisenReimer>(bsmProcess, timeSteps)));
        std::cout << "Delta equals"  << europeanOption.delta() << std::endl;
        std::cout << "Gamma equals"  << europeanOption.gamma() << std::endl;
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Joshi4>(bsmProcess, timeSteps)));
        std::cout << "Delta equals"  << europeanOption.delta() << std::endl;
        std::cout << "Gamma equals"  << europeanOption.gamma() << std::endl;
        return 0;
        
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}
