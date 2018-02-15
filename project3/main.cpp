#include "binomialtree.hpp"
#include "binomialengine.hpp"
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/pricingengines/vanilla/all.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/stochasticprocess.hpp>


#include <boost/timer.hpp>
#include <ql/time/calendars/target.hpp>
#include <stdlib.h>
#include <iostream>
#include <iomanip>


//#include <ctime>

using namespace QuantLib;

int main() {
    
    try {
        
        // Preparing Calendar
        Calendar calendar = TARGET();
        Date todaysDate(6, January, 2017);
        Date settlementDate(8, January, 2017);
        Date maturity(5, February, 2018);
        //Settings::instance().evaluationDate() = todaysDate;
        Settings::instance().evaluationDate() = todaysDate;
        DayCounter dayCounter = Actual365Fixed();
        
        // First example/First Option
        
        
        //Our option's parameters
        Option::Type type(Option::Call);
        Real strike = 50;
        Real underlying = 47;
        
        Spread dividendYield = 0.00;
        Rate riskFreeRate = 0.02;
        Volatility volatility = 0.15;
        std::string method;
        Size timeSteps=1000;

        
        std::cout << "Option type = "                   << type << std::endl;
        std::cout << "Maturity = "                      << maturity << std::endl;
        std::cout << "settlementDate = "                << settlementDate << std::endl;
        std::cout << "Strike = "                        << strike << std::endl;
        std::cout << "Underlying price = "              << underlying << std::endl;
        std::cout << "Risk-free interest rate = "       << riskFreeRate <<std::endl;
        std::cout << "Dividend yield = "                << dividendYield << std::endl;
        std::cout << "Volatility = "                    << volatility <<std::endl;
        std::cout << std::endl;
        std::cout << std::endl ;
        
        Handle<Quote> underlying_(boost::shared_ptr<Quote>(new SimpleQuote(underlying))); //underlying
        
        boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity)); // Exercise:European
        
        Handle<YieldTermStructure> flatTermStructure(boost::shared_ptr<YieldTermStructure>(
                                                                                           new FlatForward(settlementDate, riskFreeRate, dayCounter)));
        
        Handle<YieldTermStructure> flatDividendTermStructure(boost::shared_ptr<YieldTermStructure>(
                                                                                                   new FlatForward(settlementDate, dividendYield, dayCounter)));
        
        Handle<BlackVolTermStructure> flatVolTermStructure(boost::shared_ptr<BlackVolTermStructure>(
                                                                                                    new BlackConstantVol(settlementDate, calendar, volatility,dayCounter)));          //Vol
        
        boost::shared_ptr<StrikedTypePayoff> payoff(
                                                    new PlainVanillaPayoff(type, strike));                                  //Payoff
        
        boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
                                                                new BlackScholesMertonProcess(underlying_, flatDividendTermStructure,flatTermStructure, flatVolTermStructure));
        
        VanillaOption europeanOption(payoff, europeanExercise);
        
        
        //benchmark: Black&Scholes
        
        method= "Black-Scholes";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(bsmProcess)));
        Real DeltaBS=europeanOption.delta();
        Real GammaBS=europeanOption.gamma();
        std::cout << method << std::endl;
        std::cout << europeanOption.NPV() << std::endl;

        std::cout << "Delta with BS: " << DeltaBS << std::endl;
        std::cout << "Gamma with BS: " << GammaBS << std::endl;
        std::cout << std::endl;
        
        
        //JarrowRudd
        method= "JarrowRudd";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<JarrowRudd>(bsmProcess, timeSteps)));
        Real DeltaJR=europeanOption.delta();
        Real GammaJR=europeanOption.gamma();
        std::cout << method << std::endl;
        //NPV before our modifications: 2.10776
        std::cout << europeanOption.NPV() << std::endl;
        std::cout << "Delta with JR BT:            " << DeltaJR << std::endl;
        std::cout << "Gamma with JR BT:            " << GammaJR << std::endl;
        std::cout << "Difference in delta with BS: " << DeltaJR-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS: " << GammaJR-GammaBS << std::endl;
        std::cout << std::endl;
        
        
        //CoxRossRubinstein
        method= "CoxRossRubinstein";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<CoxRossRubinstein>(bsmProcess, timeSteps)));
        Real DeltaCRR=europeanOption.delta();
        Real GammaCRR=europeanOption.gamma();
        std::cout << method << std::endl;
        std::cout << "Delta with CRR BT:           " << DeltaCRR << std::endl;
        std::cout << "Gamma with CRR BT:           " << GammaCRR << std::endl;
        std::cout << "Difference in delta with BS: " << DeltaCRR-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS: " << GammaCRR-GammaBS << std::endl;
        std::cout << std::endl;
        
        //Trigeorgis
        method= "Trigeorgis";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Trigeorgis>(bsmProcess, timeSteps)));
        Real DeltaTr=europeanOption.delta();
        Real GammaTr=europeanOption.gamma();
        std::cout << method << std::endl;
        std::cout << "Delta with Tr BT:            "  << DeltaTr << std::endl;
        std::cout << "Gamma with Tr BT:            "  << GammaTr << std::endl;
        std::cout << "Difference in delta with BS: " << DeltaTr-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS: " << GammaTr-GammaBS << std::endl;
        std::cout << std::endl;
        
        //Tian
        method= "Tian";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Tian>(bsmProcess, timeSteps)));
        Real DeltaTi=europeanOption.delta();
        Real GammaTi=europeanOption.gamma();
        std::cout << method << std::endl;
        std::cout << "Delta with Ti BT:            "  << DeltaTi << std::endl;
        std::cout << "Gamma with Ti BT:            "  << GammaTi << std::endl;
        std::cout << "Difference in delta with BS: " << DeltaTi-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS: " << GammaTi-GammaBS << std::endl;
        std::cout << std::endl;
        
        //LeisenReimer
        method= "LeisenReimer";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<LeisenReimer>(bsmProcess, timeSteps)));
        Real DeltaL=europeanOption.delta();
        Real GammaL=europeanOption.gamma();
        std::cout << method << std::endl;
        std::cout << "Delta with L BT:             "  << DeltaL << std::endl;
        std::cout << "Gamma with L BT:             "  << GammaL << std::endl;
        std::cout << "Difference in delta with BS: " << DeltaL-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS: " << GammaL-GammaBS << std::endl;
        std::cout << std::endl;
        
        //Joshi4
        method= "Joshi4";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Joshi4>(bsmProcess, timeSteps)));
        Real DeltaJo=europeanOption.delta();
        Real GammaJo=europeanOption.gamma();
        std::cout << method << std::endl;
        std::cout << "Delta with Jo BT:            "  << DeltaJo << std::endl;
        std::cout << "Gamma with Jo BT:            "  << GammaJo << std::endl;
        std::cout << "Difference in delta with BS: " << DeltaJo-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS: " << GammaJo-GammaBS << std::endl;
        std::cout << std::endl;
        
        return 0;
        
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}


