
#include "binomialtree.hpp"
#include "binomialengine.hpp"
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/pricingengines/vanilla/all.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/stochasticprocess.hpp>

#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/methods/lattices/binomialtree.hpp>
#include <ql/time/calendars/target.hpp>

#include <stdlib.h>
#include <iostream>

//#include <ctime>

using namespace QuantLib;

int main() {
    
    try {
        
        // Preparing Calendar
        Calendar calendar = TARGET();
        Date todaysDate(6, January, 2017);
        Date settlementDate(8, January, 2017);
        Settings::instance().evaluationDate() = todaysDate;
        DayCounter dayCounter = Actual365Fixed();
        
        
        // First example/First Option
        
        
        //Our option's parameters
        Option::Type type(Option::Call);
        Real strike = 110;
        Real underlying = 100;
        Date maturity(5, February, 2018);

        Spread dividendYield = 0.00;
        Rate riskFreeRate = 0.03;
        Volatility volatility = 0.20;
        std::string method;
        Size timeStepsAfter=300;
        Size timeStepsBefore=300;


        std::cout << "Option type =                           " << type << std::endl;
        std::cout << "Maturity =                              " << maturity << std::endl;
        std::cout << "settlementDate =                        " << settlementDate << std::endl;
        std::cout << "Strike =                                " << strike << std::endl;
        std::cout << "Underlying price =                      " << underlying << std::endl;
        std::cout << "Risk-free interest rate =               " << riskFreeRate <<std::endl;
        std::cout << "Dividend yield =                        " << dividendYield << std::endl;
        std::cout << "Volatility =                            " << volatility <<std::endl;
        std::cout << "Time Step in the After class =          " << timeStepsAfter << std::endl;
        std::cout << "Time Step in the Before class           " << timeStepsBefore <<std::endl;
        
        std::cout << std::endl;
        std::cout << std::endl ;
        
        std::cout <<"europeanOption"<< std::endl ;
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
        std::cout << "NPV:                                    "  << europeanOption.NPV() << std::endl;
        std::cout << "Delta with BS:                          "  << DeltaBS << std::endl;
        std::cout << "Gamma with BS:                          "  << GammaBS << std::endl;
        std::cout << std::endl;
        
        
        //JarrowRudd
        //The After
        
        method= "JarrowRudd";
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        const clock_t start_time_After = clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<JarrowRudd_2>(bsmProcess, timeStepsAfter)));
        Real DeltaJR=europeanOption.delta();
        Real GammaJR=europeanOption.gamma();
        Real NPVJRAfter=europeanOption.NPV();
        std::cout << "Time spent calculating:          " <<  float( clock () - start_time_After)/CLOCKS_PER_SEC  << std::endl;

        std::cout << "NPV:                                    "  << NPVJRAfter << std::endl;
        std::cout << "Delta with JR BT:                       "  << DeltaJR << std::endl;
        std::cout << "Gamma with JR BT:                       "  << GammaJR << std::endl;
        std::cout << "Difference in delta with BS:            "  << DeltaJR-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS:            "  << GammaJR-GammaBS << std::endl;
        std::cout << std::endl;
        
        //The Before
        std::cout << "Before the modifications" << std::endl;
        
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<JarrowRudd>(bsmProcess, timeStepsBefore)));
        const clock_t start_time_Before = clock();
        Real DeltaJRBefore=europeanOption.delta();
        Real GammaJRBefore=europeanOption.gamma();
        Real NPVJRBefore=europeanOption.NPV();
        std::cout << "Time spent calculating:           " <<  float( clock () - start_time_Before)/CLOCKS_PER_SEC << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVJRBefore << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVJRBefore-NPVJRAfter << std::endl;
        std::cout << "Delta with JR BT before changes:        "  << DeltaJRBefore << std::endl;
        std::cout << "Gamma with JR BT before changes:        "  << GammaJRBefore << std::endl;
        std::cout << "Difference in delta before with BS:     "  << DeltaJRBefore-DeltaBS << std::endl;
        std::cout << "Difference in gamma before with BS:     "  << GammaJRBefore-GammaBS << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        
        
        //CoxRossRubinstein
        //The After
        method= "CoxRossRubinstein";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<CoxRossRubinstein_2>(bsmProcess, timeStepsAfter)));
        Real DeltaCRR=europeanOption.delta();
        Real GammaCRR=europeanOption.gamma();
        Real NPVCRRAfter=europeanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout <<"NPV:                                     "  << NPVCRRAfter << std::endl;
        std::cout << "Delta with CRR BT:                      "  << DeltaCRR << std::endl;
        std::cout << "Gamma with CRR BT:                      "  << GammaCRR << std::endl;
        std::cout << "Difference in delta with BS:            "  << DeltaCRR-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS:            "  << GammaCRR-GammaBS << std::endl;
        std::cout << std::endl;
        //The Before
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<CoxRossRubinstein>(bsmProcess, timeStepsBefore)));

        Real DeltaCRRBefore=europeanOption.delta();
        Real GammaCRRBefore=europeanOption.gamma();
        Real NPVCRRBefore=europeanOption.NPV();
        std::cout << "Before the modifications" << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVCRRBefore << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVCRRBefore-NPVCRRAfter << std::endl;
        std::cout << "Delta with JR BT before changes:        "  << DeltaCRRBefore << std::endl;
        std::cout << "Gamma with JR BT before changes:        "  << GammaCRRBefore << std::endl;
        std::cout << "Difference in delta before with BS:     "  << DeltaCRRBefore-DeltaBS << std::endl;
        std::cout << "Difference in gamma before with BS:     "  << GammaCRRBefore-GammaBS << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        
        //Trigeorgis
        //The After
        method= "Trigeorgis";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<Trigeorgis_2>(bsmProcess, timeStepsAfter)));
        Real DeltaTr=europeanOption.delta();
        Real GammaTr=europeanOption.gamma();
        Real NPVTrBefore=europeanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV:                                    "  << NPVTrBefore << std::endl;
        std::cout << "Delta with Tr BT:                       "  << DeltaTr << std::endl;
        std::cout << "Gamma with Tr BT:                       "  << GammaTr << std::endl;
        std::cout << "Difference in delta with BS:            "  << DeltaTr-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS:            "  << GammaTr-GammaBS << std::endl;
        std::cout << std::endl;
        //The Before
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Trigeorgis>(bsmProcess, timeStepsBefore)));

        Real DeltaTrBefore=europeanOption.delta();
        Real GammaTrBefore=europeanOption.gamma();
        Real NPVTrAfter=europeanOption.NPV();

        std::cout << "Before the modifications" << std::endl;       
        std::cout << "NPV Before changes:                     "  << NPVTrBefore << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVTrBefore-NPVTrAfter << std::endl;
        std::cout << "Delta with JR BT before changes:        "  << DeltaTrBefore << std::endl;
        std::cout << "Gamma with JR BT before changes:        "  << GammaTrBefore << std::endl;
        std::cout << "Difference in delta before with BS:     "  << DeltaTrBefore-DeltaBS << std::endl;
        std::cout << "Difference in gamma before with BS:     "  << GammaTrBefore-GammaBS << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
       
        
        //Tian
        //The After
        method= "Tian";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<Tian_2>(bsmProcess, timeStepsAfter)));
        Real DeltaTi=europeanOption.delta();
        Real GammaTi=europeanOption.gamma();
        Real NPVTiAfter=europeanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV:                                    "  << NPVTiAfter << std::endl;
        std::cout << "Delta with Ti BT:                       "  << DeltaTi << std::endl;
        std::cout << "Gamma with Ti BT:                       "  << GammaTi << std::endl;
        std::cout << "Difference in delta with BS:            "  << DeltaTi-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS:            "  << GammaTi-GammaBS << std::endl;
        std::cout << std::endl;
        
        //The Before
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Tian>(bsmProcess, timeStepsBefore)));
        Real DeltaTiBefore=europeanOption.delta();
        Real GammaTiBefore=europeanOption.gamma();
        Real NPVTiBefore=europeanOption.NPV();
        std::cout << "Before the modifications" << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVTiBefore << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVTiBefore-NPVTiAfter << std::endl;
        std::cout << "Delta with JR BT before changes:        "  << DeltaTiBefore << std::endl;
        std::cout << "Gamma with JR BT before changes:        "  << GammaTiBefore << std::endl;
        std::cout << "Difference in delta before with BS:     "  << DeltaTiBefore-DeltaBS << std::endl;
        std::cout << "Difference in gamma before with BS:     "  << GammaTiBefore-GammaBS << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        
        
        //LeisenReimer
        //The After
        method= "LeisenReimer";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<LeisenReimer_2>(bsmProcess, timeStepsAfter)));
        Real DeltaL=europeanOption.delta();
        Real GammaL=europeanOption.gamma();
        Real NPVLAfter=europeanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV:                                    "  << NPVLAfter << std::endl;
        std::cout << "Delta with L BT:                        "  << DeltaL << std::endl;
        std::cout << "Gamma with L BT:                        "  << GammaL << std::endl;
        std::cout << "Difference in delta with BS:            "  << DeltaL-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS:           "  << GammaL-GammaBS << std::endl;
        std::cout << std::endl;
        
        //The Before
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<LeisenReimer>(bsmProcess, timeStepsBefore)));
        Real DeltaLBefore=europeanOption.delta();
        Real GammaLBefore=europeanOption.gamma();
        Real NPVLBefore=europeanOption.NPV();
        std::cout << "Before the modifications" << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVLBefore << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVLBefore-NPVLAfter << std::endl;
        std::cout << "Delta with JR BT before changes:        "  << DeltaLBefore << std::endl;
        std::cout << "Gamma with JR BT before changes:        "  << GammaLBefore << std::endl;
        std::cout << "Difference in delta before with BS:     "  << DeltaLBefore-DeltaBS << std::endl;
        std::cout << "Difference in gamma before with BS:     "  << GammaLBefore-GammaBS << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        

        //Joshi4
        //The After
        method= "Joshi4";
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<Joshi4_2>(bsmProcess, timeStepsAfter)));
        Real DeltaJo=europeanOption.delta();
        Real GammaJo=europeanOption.gamma();
        Real NPVJoAfter=europeanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV:                                    "  << NPVJoAfter << std::endl;
        std::cout << "Delta with Jo BT:                       "  << DeltaJo << std::endl;
        std::cout << "Gamma with Jo BT:                       "  << GammaJo << std::endl;
        std::cout << "Difference in delta with BS:            "  << DeltaJo-DeltaBS << std::endl;
        std::cout << "Difference in gamma with BS:            "  << GammaJo-GammaBS << std::endl;
        std::cout << std::endl;
        //The Before
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Joshi4>(bsmProcess, timeStepsBefore)));
        Real DeltaJoBefore=europeanOption.delta();
        Real GammaJoBefore=europeanOption.gamma();
        Real NPVJoBefore=europeanOption.NPV();
        std::cout << "Before the modifications" << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVJoBefore << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVJoBefore-NPVJoAfter << std::endl;
        std::cout << "Delta with JR BT before changes:        "  << DeltaJoBefore << std::endl;
        std::cout << "Gamma with JR BT before changes:        "  << GammaJoBefore << std::endl;
        std::cout << "Difference in delta before with BS:     "  << DeltaJoBefore-DeltaBS << std::endl;
        std::cout << "Difference in gamma before with BS:     "  << GammaJoBefore-GammaBS << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        
      
        
        
        // Second example/Second Option

        
        
        
        std::cout <<"americanOption" << std::endl;
        boost::shared_ptr<Exercise> americanExercise(new AmericanExercise(maturity)); // Exercise:American
        VanillaOption americanOption(payoff, americanExercise);
        method= "BaroneAdesiWhaleyApproximation";
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BaroneAdesiWhaleyApproximationEngine(bsmProcess)));
        Real DeltaAW=americanOption.delta();
        Real GammaAW=americanOption.gamma();
        std::cout << method << std::endl;
        std::cout << "NPV:                                    "  << americanOption.NPV() << std::endl;
        std::cout << "Delta with BaroneAdesiWhaley :          "  << DeltaAW << std::endl;
        std::cout << "Gamma with BaroneAdesiWhaley:           "  << GammaAW << std::endl;
        std::cout << std::endl;
        
        
        //JarrowRudd
        //The After
        method= "JarrowRudd";
        std::cout << method << std::endl;
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<JarrowRudd_2>(bsmProcess, timeStepsAfter)));
        Real DeltaJRa=americanOption.delta();
        Real GammaJRa=americanOption.gamma();
        Real NPVJRAfter_a=americanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVJRAfter_a << std::endl;
        std::cout << "Delta with JR BT:                       "  << DeltaJRa << std::endl;
        std::cout << "Gamma with JR BT:                       "  << GammaJRa << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaJRa-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaJRa-GammaAW << std::endl;
        std::cout << std::endl;
        //The Before
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<JarrowRudd>(bsmProcess, timeStepsBefore)));
        Real DeltaJRBefore_a=americanOption.delta();
        Real GammaJRBefore_a=americanOption.gamma();
        Real NPVJRBefore_a=americanOption.NPV();
        std::cout << "Before the modifications" << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVJRBefore_a << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVJRBefore_a-NPVJRAfter_a << std::endl;
        std::cout << "Delta with JR BT before changes:        "  << DeltaJRBefore_a << std::endl;
        std::cout << "Gamma with JR BT before changes:        "  << GammaJRBefore_a << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaJRBefore_a-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaJRBefore_a-GammaAW << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        
        
        
        //CoxRossRubinstein
        //The after
        method= "CoxRossRubinstein";
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<CoxRossRubinstein_2>(bsmProcess, timeStepsAfter)));
        Real DeltaCRRa=americanOption.delta();
        Real GammaCRRa=americanOption.gamma();
        Real NPVCRRAfter_a=americanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV:                                    "  << americanOption.NPV() << std::endl;
        std::cout << "Delta with CRR BT:                      "  << DeltaCRRa << std::endl;
        std::cout << "Gamma with CRR BT:                      "  << GammaCRRa << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaCRRa-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaCRRa-GammaAW << std::endl;
        std::cout << std::endl;
        //The Before
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<CoxRossRubinstein>(bsmProcess, timeStepsBefore)));
        Real DeltaCRRBefore_a=americanOption.delta();
        Real GammaCRRBefore_a=americanOption.gamma();
        Real NPVCRRBefore_a=americanOption.NPV();
        std::cout << "Before the modifications" << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVCRRBefore_a << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVCRRBefore_a-NPVCRRAfter_a << std::endl;
        std::cout << "Delta with CRR BT before changes:       "  << DeltaCRRBefore_a << std::endl;
        std::cout << "Gamma with CRR BT before changes:       "  << GammaCRRBefore_a << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaCRRBefore_a-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaCRRBefore_a-GammaAW << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        
        //Trigeorgis
        //The After
        method= "Trigeorgis";
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<Trigeorgis_2>(bsmProcess, timeStepsAfter)));
        Real DeltaTra=americanOption.delta();
        Real GammaTra=americanOption.gamma();
        Real NPVTrAfter_a=americanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV:                                    "  << NPVTrAfter_a << std::endl;
        std::cout << "Delta with Tr BT:                       "  << DeltaTra << std::endl;
        std::cout << "Gamma with Tr BT:                       "  << GammaTra << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaTra-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaTra-GammaAW << std::endl;
        std::cout << std::endl;
        //The Before
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Trigeorgis>(bsmProcess, timeStepsBefore)));
        Real DeltaTrBefore_a=americanOption.delta();
        Real GammaTrBefore_a=americanOption.gamma();
        Real NPVTrBefore_a=americanOption.NPV();
        std::cout << "Before the modifications" << std::endl;        
        std::cout << "NPV Before changes:                     "  << NPVTrBefore_a << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVTrBefore_a-NPVTrAfter_a << std::endl;
        std::cout << "Delta with Tr BT before changes:        "  << DeltaTrBefore_a << std::endl;
        std::cout << "Gamma with Tr BT before changes:        "  << GammaTrBefore_a << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaTrBefore_a-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaTrBefore_a-GammaAW << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        
        
        
        //Tian
        //The After
        method= "Tian";
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<Tian_2>(bsmProcess, timeStepsAfter)));
        Real DeltaTia=americanOption.delta();
        Real GammaTia=americanOption.gamma();
        Real NPVTiAfter_a=americanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV:                                    "  << NPVTiAfter_a << std::endl;
        std::cout << "Delta with Ti BT:                       "  << DeltaTia << std::endl;
        std::cout << "Gamma with Ti BT:                       "  << GammaTia << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaTia-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaTia-GammaAW << std::endl;
        std::cout << std::endl;

        //The Before
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Tian>(bsmProcess, timeStepsBefore)));
        Real DeltaTiBefore_a=americanOption.delta();
        Real GammaTiBefore_a=americanOption.gamma();
        Real NPVTiBefore_a=americanOption.NPV();
        std::cout << "Before the modifications" << std::endl;        
        std::cout << "NPV Before changes:                     "  << NPVTiBefore_a << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVTiBefore_a-NPVTiAfter_a << std::endl;
        std::cout << "Delta with Ti BT before changes:        "  << DeltaTiBefore_a << std::endl;
        std::cout << "Gamma with Ti BT before changes:        "  << GammaTiBefore_a << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaTiBefore_a-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaTiBefore_a-GammaAW << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        
        
        
        //LeisenReimer
        method= "LeisenReimer";
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<LeisenReimer_2>(bsmProcess, timeStepsAfter)));
        Real DeltaLaa=americanOption.delta();
        Real GammaLaa=americanOption.gamma();
        Real NPVLaAfter_a=americanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV:                                    "  << NPVLaAfter_a << std::endl;
        std::cout << "Delta with Li BT:                       "  << DeltaLaa << std::endl;
        std::cout << "Gamma with Li BT:                       "  << GammaLaa << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaLaa-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaLaa-GammaAW << std::endl;
        std::cout << std::endl;
        
        //The Before
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<LeisenReimer>(bsmProcess, timeStepsBefore)));
        Real DeltaLaBefore_a=americanOption.delta();
        Real GammaLaBefore_a=americanOption.gamma();
        Real NPVLaBefore_a=americanOption.NPV();
        std::cout << "Before the modifications" << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVLaBefore_a << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVLaBefore_a-NPVLaAfter_a << std::endl;
        std::cout << "Delta with Li BT before changes:        "  << DeltaLaBefore_a << std::endl;
        std::cout << "Gamma with Li BT before changes:        "  << GammaLaBefore_a << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaLaBefore_a-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaLaBefore_a-GammaAW << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        
        
        //Joshi4
        method= "Joshi4";
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine_2<Joshi4_2>(bsmProcess, timeStepsAfter)));
        Real DeltaJoa=americanOption.delta();
        Real GammaJoa=americanOption.gamma();
        Real NPVJoAfter_a=americanOption.NPV();
        std::cout << method << std::endl;
        std::cout << std::endl;
        std::cout << "After the modifications" << std::endl;
        std::cout << "NPV:                                    "  << NPVJoAfter_a << std::endl;
        std::cout << "Delta with Jo BT:                       "  << DeltaJoa << std::endl;
        std::cout << "Gamma with Jo BT:                       "  << GammaJoa << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaJoa-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaJoa-GammaAW << std::endl;

        std::cout << std::endl;
        
        //The Before
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>( new BinomialVanillaEngine<Joshi4>(bsmProcess, timeStepsBefore)));
        Real DeltaJoBefore_a=americanOption.delta();
        Real GammaJoBefore_a=americanOption.gamma();
        Real NPVJoBefore_a=americanOption.NPV();
        std::cout << "Before the modifications" << std::endl;
        std::cout << "NPV Before changes:                     "  << NPVJoBefore_a << std::endl;
        std::cout << "Difference in NPV after and before:     "  << NPVJoBefore_a-NPVLaAfter_a << std::endl;
        std::cout << "Delta with Jo BT before changes:        "  << DeltaJoBefore_a << std::endl;
        std::cout << "Gamma with Jo BT before changes:        "  << GammaJoBefore_a << std::endl;
        std::cout << "Difference in delta with WA:            "  << DeltaJoBefore_a-DeltaAW << std::endl;
        std::cout << "Difference in gamma with WA:            "  << GammaJoBefore_a-GammaAW << std::endl;
        std::cout << std::endl;
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
