///
/// CppUnit tests for the extrapolation tools.
///

#include "Combination/ExtrapolationTools.h"
#include "Combination/CalibrationDataModelStreams.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>

#include <stdexcept>

using namespace std;
using namespace BTagCombination;

class ExtrapolationToolsTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (ExtrapolationToolsTest );

  CPPUNIT_TEST_EXCEPTION (testExtrapolate1binPtHighNoErrors, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION (testExtrapolate1binPtHighNoErrors2, std::runtime_error);

  CPPUNIT_TEST (testExtrapolate1binPtHighErrors);
  CPPUNIT_TEST (testExtrapolate1binPtHighErrors2);

  CPPUNIT_TEST(testExtrapolateLinage);

  CPPUNIT_TEST (testExtrapolate1binPtHigh);
  CPPUNIT_TEST (testExtrapolate1binPtLow);
  CPPUNIT_TEST (testExtrapolateWithMulitpleEtaBins);
  CPPUNIT_TEST (testNoExtrapolation);
  CPPUNIT_TEST (testExtrapolate1binPtHigh2ExtBins);
  CPPUNIT_TEST (testExtrapolate1binExactlyZero);
  CPPUNIT_TEST (testExtrapolate1bin2CorErrors);
  CPPUNIT_TEST(testExtrapolate1bin2CorErrors_nohigher);
  CPPUNIT_TEST(testExtrapolate1bin2CorErrors_nolower);
  CPPUNIT_TEST(testExtrapolate1bin2ACorErrors);
  CPPUNIT_TEST (testExtrapolate1bin2CorErrorsBin1);
  CPPUNIT_TEST (testExtrapolate1binNegative);

  // Check the central value and stat error
  CPPUNIT_TEST (testExtrapolate1binCV);

  // Bad extrapolation binning - 2D extension, gaps, overlaps, etc.
  CPPUNIT_TEST_EXCEPTION (testExtrapolate1binPtAndEta, runtime_error);
  CPPUNIT_TEST_EXCEPTION (testExtrapolate1binPtGap, runtime_error);
  CPPUNIT_TEST_EXCEPTION (testExtrapolate1binPtOverlap, runtime_error);

  // Dummy x-checks
  CPPUNIT_TEST_EXCEPTION (testExtrapolateTwice, runtime_error);

  // Fail b.c. we don't support extrapolating irregular binning
  CPPUNIT_TEST_EXCEPTION (testExtrapolateWithMulitpleEtaBinsWithSingleExtrapolationBin, runtime_error);
  CPPUNIT_TEST_EXCEPTION (testExtrapolationWithSingleEtaBinWithMultipleExtrapolation, runtime_error);
  CPPUNIT_TEST_EXCEPTION(testExtrapolationWithSingleEtaBinWithMultipleSecondLevelExtrapolation, runtime_error);

  CPPUNIT_TEST_SUITE_END();

  CalibrationAnalysis generate_1bin_ana()
  {
    // Simple 1 bin analysis
    CalibrationAnalysis ana;
    ana.name = "ana1";
    ana.flavor = "bottom";
    ana.tagger = "SV0";
    ana.operatingPoint = "0.1";
    ana.jetAlgorithm = "AntiKt";

    CalibrationBin b1;
    b1.centralValue = 1.1;
    b1.centralValueStatisticalError = 0.2;
    CalibrationBinBoundary bb1;
    bb1.lowvalue = 0.0;
    bb1.highvalue = 100.0;
    bb1.variable = "pt";
    b1.binSpec.push_back(bb1);
    bb1.lowvalue = 0.0;
    bb1.highvalue = 2.5;
    bb1.variable = "abseta";
    b1.binSpec.push_back(bb1);

    SystematicError e;
    e.name = "err";
    e.value = 0.1;
    e.uncorrelated = false;
    b1.systematicErrors.push_back(e);
    
    ana.bins.push_back(b1);
    return ana;
  }

  CalibrationAnalysis generate_2bin_ana()
  {
    // Simple 2 bin analysis
    CalibrationAnalysis ana;
    ana.name = "ana1";
    ana.flavor = "bottom";
    ana.tagger = "SV0";
    ana.operatingPoint = "0.1";
    ana.jetAlgorithm = "AntiKt";

    CalibrationBin b1;
    b1.centralValue = 1.1;
    b1.centralValueStatisticalError = 0.2;
    CalibrationBinBoundary bb1;
    bb1.lowvalue = 0.0;
    bb1.highvalue = 100.0;
    bb1.variable = "pt";
    b1.binSpec.push_back(bb1);
    bb1.lowvalue = 0.0;
    bb1.highvalue = 2.5;
    bb1.variable = "abseta";
    b1.binSpec.push_back(bb1);

    SystematicError e;
    e.name = "err";
    e.value = 0.1;
    e.uncorrelated = false;
    b1.systematicErrors.push_back(e);
    
    ana.bins.push_back(b1);

    /// Second bin looks a lot like the first bin!
    b1.binSpec[0].lowvalue = 100.0;
    b1.binSpec[0].highvalue = 200.0;
    ana.bins.push_back(b1);

    return ana;
  }

  CalibrationAnalysis generate_2bin_2eta_ana()
  {
    // Simple 1 bin analysis
    CalibrationAnalysis ana;
    ana.name = "ana1";
    ana.flavor = "bottom";
    ana.tagger = "SV0";
    ana.operatingPoint = "0.1";
    ana.jetAlgorithm = "AntiKt";

    CalibrationBin b1;
    b1.centralValue = 1.1;
    b1.centralValueStatisticalError = 0.2;
    CalibrationBinBoundary bb1;
    bb1.lowvalue = 0.0;
    bb1.highvalue = 100.0;
    bb1.variable = "pt";
    b1.binSpec.push_back(bb1);
    bb1.lowvalue = 0.0;
    bb1.highvalue = 2.5;
    bb1.variable = "abseta";
    b1.binSpec.push_back(bb1);

    SystematicError e;
    e.name = "err";
    e.value = 0.1;
    e.uncorrelated = false;
    b1.systematicErrors.push_back(e);
    
    ana.bins.push_back(b1);

    b1.binSpec[1].lowvalue = 2.5;
    b1.binSpec[1].highvalue = 4.0;
    ana.bins.push_back(b1);

    return ana;
  }

  CalibrationAnalysis generate_2bin_extrap_in_pthigh()
  {
    // Extend the standard 1 bin in pt on the high side
    // Simple 1 bin analysis
    CalibrationAnalysis ana;
    ana.name = "ana1";
    ana.flavor = "bottom";
    ana.tagger = "SV0";
    ana.operatingPoint = "0.1";
    ana.jetAlgorithm = "AntiKt";

    CalibrationBin b1;
    b1.centralValue = 1.3;
    b1.centralValueStatisticalError = 0.4;
    CalibrationBinBoundary bb1;
    bb1.lowvalue = 0.0;
    bb1.highvalue = 100.0;
    bb1.variable = "pt";
    b1.binSpec.push_back(bb1);
    bb1.lowvalue = 0.0;
    bb1.highvalue = 2.5;
    bb1.variable = "abseta";
    b1.binSpec.push_back(bb1);

    SystematicError e;
    e.name = "extr";
    e.value = 0.1;
    e.uncorrelated = false;
    b1.systematicErrors.push_back(e);
    ana.bins.push_back(b1);

    b1.binSpec[0].lowvalue = 100.0;
    b1.binSpec[0].highvalue = 200.0;
    b1.systematicErrors[0].value = 0.2; // x2 error size
    ana.bins.push_back(b1);

    return ana;
  }

  CalibrationAnalysis generate_2bin_extrap_in_pthigh_ZeroErrors(int nerr = 1, double eval1 = 0.1, double eval2 = 0.0)
  {
    // Extend the standard 1 bin in pt on the high side
    // Simple 1 bin analysis
    CalibrationAnalysis ana;
    ana.name = "ana1";
    ana.flavor = "bottom";
    ana.tagger = "SV0";
    ana.operatingPoint = "0.1";
    ana.jetAlgorithm = "AntiKt";

    CalibrationBin b1;
    b1.centralValue = 1.3;
    b1.centralValueStatisticalError = 1.0;
    CalibrationBinBoundary bb1;
    bb1.lowvalue = 0.0;
    bb1.highvalue = 100.0;
    bb1.variable = "pt";
    b1.binSpec.push_back(bb1);
    bb1.lowvalue = 0.0;
    bb1.highvalue = 2.5;
    bb1.variable = "abseta";
    b1.binSpec.push_back(bb1);

    for (int i = 0; i < nerr; i++) {
      SystematicError e;
      ostringstream ename;
      ename << "extr_" << i;
      e.name = ename.str();
      e.value = eval1;
      e.uncorrelated = false;
      b1.systematicErrors.push_back(e);
    }
    ana.bins.push_back(b1);

    b1.binSpec[0].lowvalue = 100.0;
    b1.binSpec[0].highvalue = 200.0;
    b1.systematicErrors.clear();
    for (int i = 0; i < nerr; i++) {
      SystematicError e;
      ostringstream ename;
      ename << "extr_" << i;
      e.name = ename.str();
      e.value = eval2;
      e.uncorrelated = false;
      b1.systematicErrors.push_back(e);
    }
    ana.bins.push_back(b1);

    return ana;
  }

  CalibrationAnalysis generate_2bin_extrap_in_pthigh_2cerrors()
  {
    // Extend the standard 1 bin in pt on the high side
    // Simple 1 bin analysis
    CalibrationAnalysis ana(generate_2bin_extrap_in_pthigh());
    SystematicError second;
    second.name = "hi";
    second.value = 0.2; // x2
    ana.bins[1].systematicErrors.push_back(second);
    second.value = 0.1;
    ana.bins[0].systematicErrors.push_back(second);
    return ana;
  }

  CalibrationAnalysis generate_2bin_extrap_in_pthigh_2cerrors_no_first_bin_2nd_error()
  {
    // Extend the standard 1 bin in pt on the high side
    // Simple 1 bin analysis
    CalibrationAnalysis ana(generate_2bin_extrap_in_pthigh());
    SystematicError second;
    second.name = "hi";
    second.value = 0.2; // x2
    ana.bins[1].systematicErrors.push_back(second);
    return ana;
  }

  CalibrationAnalysis generate_2bin_extrap_in_pthigh_2cerrors_no_second_bin_2nd_error()
  {
    // Extend the standard 1 bin in pt on the high side
    // Simple 1 bin analysis
    CalibrationAnalysis ana(generate_2bin_extrap_in_pthigh());
    SystematicError second;
    second.name = "hi";
    second.value = 0.1; // x2
    ana.bins[0].systematicErrors.push_back(second);
    return ana;
  }

  CalibrationAnalysis generate_2bin_extrap_in_pthigh_2acerrors()
  {
    CalibrationAnalysis ana(generate_2bin_extrap_in_pthigh());
    SystematicError second;
    second.name = "hi";
    second.value = -0.2; // x2
    ana.bins[1].systematicErrors.push_back(second);
    second.value = 0.1;
    ana.bins[0].systematicErrors.push_back(second);
    return ana;
  }

  CalibrationAnalysis generate_2bin_extrap_in_pthigh_2cerrorsbin1()
  {
    CalibrationAnalysis ana(generate_2bin_extrap_in_pthigh());
    SystematicError second;
    second.name = "hi";
    second.value = 0.1; // x2
    ana.bins[0].systematicErrors.push_back(second);
    return ana;
  }

  CalibrationAnalysis generate_3bin_extrap_in_pthigh()
  {
    CalibrationAnalysis ana(generate_2bin_extrap_in_pthigh());
    CalibrationBin b(ana.bins[1]);
    b.binSpec[0].lowvalue = 200.0;
    b.binSpec[0].highvalue = 300.0;
    b.systematicErrors[0].value *= 2; // x2 larger
    ana.bins.push_back(b);
    return ana;
  }

  CalibrationAnalysis generate_2bin_extrap_in_pteta()
  {
    // Extend the standard 1 bin in pt on the high side and on the eta side.
    // This is no legal with the current CDI and "what to do" limitations.
    CalibrationAnalysis ana;
    ana.name = "ana1";
    ana.flavor = "bottom";
    ana.tagger = "SV0";
    ana.operatingPoint = "0.1";
    ana.jetAlgorithm = "AntiKt";

    CalibrationBin b1;
    b1.centralValue = 1.1;
    b1.centralValueStatisticalError = 0.2;
    CalibrationBinBoundary bb1;
    bb1.lowvalue = 0.0;
    bb1.highvalue = 100.0;
    bb1.variable = "pt";
    b1.binSpec.push_back(bb1);
    bb1.lowvalue = 0.0;
    bb1.highvalue = 2.5;
    bb1.variable = "abseta";
    b1.binSpec.push_back(bb1);

    SystematicError e;
    e.name = "extr";
    e.value = 0.1;
    e.uncorrelated = false;
    b1.systematicErrors.push_back(e);
    ana.bins.push_back(b1);

    b1.binSpec[0].lowvalue = 100.0;
    b1.binSpec[0].highvalue = 200.0;
    b1.systematicErrors[0].value = 0.2; // x2 error size
    ana.bins.push_back(b1);

    b1.binSpec[1].lowvalue = 2.5;
    b1.binSpec[1].highvalue = 4.0;
    b1.systematicErrors[0].value = 0.2; // x2 error size
    ana.bins.push_back(b1);

    return ana;
  }

  // Extend pt on the low side.
  CalibrationAnalysis generate_2bin_extrap_in_ptlow()
  {
    CalibrationAnalysis ana(generate_2bin_extrap_in_pthigh());
    ana.bins[1].binSpec[0].lowvalue = -100.0;
    ana.bins[1].binSpec[0].highvalue = 0.0;
    return ana;
  }

  CalibrationAnalysis generate_2bin_2eta_extrap_in_pthigh()
  {
    // Extend the standard 1 bin in pt on the high side
    // Simple 1 bin analysis
    CalibrationAnalysis ana;
    ana.name = "ana1";
    ana.flavor = "bottom";
    ana.tagger = "SV0";
    ana.operatingPoint = "0.1";
    ana.jetAlgorithm = "AntiKt";

    CalibrationBin b1;
    b1.centralValue = 1.1;
    b1.centralValueStatisticalError = 0.2;
    CalibrationBinBoundary bb1;
    bb1.lowvalue = 0.0;
    bb1.highvalue = 100.0;
    bb1.variable = "pt";
    b1.binSpec.push_back(bb1);
    bb1.lowvalue = 0.0;
    bb1.highvalue = 2.5;
    bb1.variable = "abseta";
    b1.binSpec.push_back(bb1);

    SystematicError e;
    e.name = "extr";
    e.value = 0.1;
    e.uncorrelated = false;
    b1.systematicErrors.push_back(e);
    ana.bins.push_back(b1);

    // high eta bin
    b1.binSpec[1].lowvalue = 2.5;
    b1.binSpec[1].highvalue = 4.0;
    ana.bins.push_back(b1);

    // pt extension
    b1.binSpec[0].lowvalue = 100.0;
    b1.binSpec[0].highvalue = 200.0;
    b1.systematicErrors[0].value = 0.2; // x2 error size
    ana.bins.push_back(b1);

    // low eta extension
    b1.binSpec[1].lowvalue = 0.0;
    b1.binSpec[1].highvalue = 2.5;
    ana.bins.push_back(b1);
    
    return ana;
  }

  CalibrationAnalysis generate_2bin_1then2eta_extrap_in_pthigh()
  {
    // Extend the standard 1 bin in pt on the high side
    // Simple 1 bin analysis
    CalibrationAnalysis ana;
    ana.name = "ana1";
    ana.flavor = "bottom";
    ana.tagger = "SV0";
    ana.operatingPoint = "0.1";
    ana.jetAlgorithm = "AntiKt";

    CalibrationBin b1;
    b1.centralValue = 1.1;
    b1.centralValueStatisticalError = 0.2;
    CalibrationBinBoundary bb1;
    bb1.lowvalue = 0.0;
    bb1.highvalue = 100.0;
    bb1.variable = "pt";
    b1.binSpec.push_back(bb1);
    bb1.lowvalue = 0.0;
    bb1.highvalue = 4.0;
    bb1.variable = "abseta";
    b1.binSpec.push_back(bb1);

    SystematicError e;
    e.name = "extr";
    e.value = 0.1;
    e.uncorrelated = false;
    b1.systematicErrors.push_back(e);
    ana.bins.push_back(b1);

    // pt extension
    b1.binSpec[0].lowvalue = 100.0;
    b1.binSpec[0].highvalue = 200.0;
    b1.systematicErrors[0].value = 0.2; // x2 error size
    b1.binSpec[1].lowvalue = 0.0;
    b1.binSpec[1].highvalue = 2.5;
    ana.bins.push_back(b1);

    // low eta extension
    b1.binSpec[1].lowvalue = 2.5;
    b1.binSpec[1].highvalue = 4.0;
    ana.bins.push_back(b1);
    
    return ana;
  }

  // Do the pt extrapolation, on the high side.
  void testExtrapolate1binPtHigh()
  {
    cout << "Starting testExtrapolate1binPtHigh()" << endl;
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));

    CPPUNIT_ASSERT_EQUAL(size_t(2), result.bins.size());
    CPPUNIT_ASSERT (result.bins[1].isExtended);

    // First bin error should remain untouched
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[0].systematicErrors.size());
    SystematicError e1(result.bins[0].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("err"), e1.name);
    CPPUNIT_ASSERT_EQUAL(double(0.1), e1.value);

    // Extrapolated bins have only one error
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[1].systematicErrors.size());
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e2.name);

    // Doubles in size from the first one.
    // The calibration data interface (Frank) figures out the total, but will add in quad with the other errors,
    // so a funny quad subtraction occurs.
    //CPPUNIT_ASSERT_EQUAL(sqrt(0.2*0.2-0.1*0.1), e2.value); // Run 1
    CPPUNIT_ASSERT_EQUAL(0.1, e2.value); // Run 2
  }

  // Do the pt extrapolation, on the high side.
  void testExtrapolateLinage()
  {
    CalibrationAnalysis ana(generate_1bin_ana());
    ana.name = "ana";
    CalibrationAnalysis extrap(generate_2bin_extrap_in_pthigh());
    extrap.name = "MCCalib";

    CalibrationAnalysis result(addExtrapolation(extrap, ana));
    CPPUNIT_ASSERT_EQUAL(string("ana+extrap[MCCalib]"), result.metadata_s["Linage"]);
  }

  // Do the pt extrapolation, on the high side.
  void testExtrapolate1binPtHighNoErrors()
  {
    cout << "Starting testExtrapolate1binPtHighNoErrors()" << endl;
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh_ZeroErrors());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // Do the pt extrapolation, on the high side.
  void testExtrapolate1binPtHighNoErrors2()
  {
    cout << "Starting testExtrapolate1binPtHighNoErrors()" << endl;
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh_ZeroErrors(10));

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // Do the pt extrapolation, on the high side.
  void testExtrapolate1binPtHighErrors()
  {
    cout << "Starting testExtrapolate1binPtHighNoErrors()" << endl;
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh_ZeroErrors(1, 0.1, 0.1));

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // Do the pt extrapolation, on the high side.
  void testExtrapolate1binPtHighErrors2()
  {
    cout << "Starting testExtrapolate1binPtHighNoErrors()" << endl;
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh_ZeroErrors(10, 0.1, 0.1));

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // Make sure the cv is set properly.
  void testExtrapolate1binCV()
  {
    cout << "Starting testExtrapolate1binCV()" << endl;
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));

    CPPUNIT_ASSERT_EQUAL(size_t(2), result.bins.size());
    CPPUNIT_ASSERT (result.bins[1].isExtended);

    // The central value 
    CPPUNIT_ASSERT_DOUBLES_EQUAL(result.bins[0].centralValue, result.bins[1].centralValue, 0.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(result.bins[0].centralValueStatisticalError, result.bins[1].centralValueStatisticalError, 0.01);

    // Bin boundaries
    CalibrationBinBoundary b1 (result.bins[0].binSpec[0]);
    CalibrationBinBoundary bext (result.bins[1].binSpec[0]);

    CPPUNIT_ASSERT_EQUAL(b1.variable, bext.variable);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0, b1.highvalue, 0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0, bext.lowvalue, 0.1);
  }

  // The extrapolation has multiple correlated errors in in both bins.
  void testExtrapolate1bin2CorErrors()
  {
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh_2cerrors());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));

    // First bin error should remain untouched
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[0].systematicErrors.size());
    SystematicError e1(result.bins[0].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("err"), e1.name);
    CPPUNIT_ASSERT_EQUAL(double(0.1), e1.value);

    // Extrapolated bins have only one error
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[1].systematicErrors.size());
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e2.name);

    // Doubles in size from the first one.
    // The extrapolation figures out the total, but will add in quad with the other errors,
    // so a funny quad subtraction occurs.
    CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(2)*0.1, e2.value, 0.0001);
  }

  // The extrapolation has multiple correlated errors in in both bins.
  void testExtrapolate1bin2CorErrors_nolower()
  {
    CalibrationAnalysis ana(generate_1bin_ana());
    CalibrationAnalysis extrap(generate_2bin_extrap_in_pthigh_2cerrors_no_first_bin_2nd_error());

    CalibrationAnalysis result(addExtrapolation(extrap, ana));

    // First bin error should remain untouched
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[0].systematicErrors.size());
    SystematicError e1(result.bins[0].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("err"), e1.name);
    CPPUNIT_ASSERT_EQUAL(double(0.1), e1.value);

    // Extrapolated bins have only one error
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[1].systematicErrors.size());
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e2.name);

    // Doubles in size from the first one.
    // The extrapolation figures out the total, but will add in quad with the other errors,
    // so a funny quad subtraction occurs.
    CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(0.1*0.1 + 0.2*0.2), e2.value, 0.0001);
  }

  // The extrapolation has multiple correlated errors in in both bins.
  void testExtrapolate1bin2CorErrors_nohigher()
  {
    CalibrationAnalysis ana(generate_1bin_ana());
    CalibrationAnalysis extrap(generate_2bin_extrap_in_pthigh_2cerrors_no_second_bin_2nd_error());

    CalibrationAnalysis result(addExtrapolation(extrap, ana));

    // First bin error should remain untouched
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[0].systematicErrors.size());
    SystematicError e1(result.bins[0].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("err"), e1.name);
    CPPUNIT_ASSERT_EQUAL(double(0.1), e1.value);

    // Extrapolated bins have only one error
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[1].systematicErrors.size());
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e2.name);

    // Doubles in size from the first one.
    // The extrapolation figures out the total, but will add in quad with the other errors,
    // so a funny quad subtraction occurs.
    CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(2)*0.1, e2.value, 0.0001);
  }

  // See what happens with multiple a-correlated errors.
  void testExtrapolate1bin2ACorErrors()
  {
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh_2acerrors());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));

    // First bin error should remain untouched
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[0].systematicErrors.size());
    SystematicError e1(result.bins[0].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("err"), e1.name);
    CPPUNIT_ASSERT_EQUAL(double(0.1), e1.value);

    // Extrapolated bins have only one error
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[1].systematicErrors.size());
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e2.name);

    // Doubles in size from the first one.
    // The extrapolation figures out the total, but will add in quad with the other errors,
    // so a funny quad subtraction occurs.
    CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(2)*0.1, e2.value, 0.0001);
  }

  // Two errors in first bin, one error in second bin.
  void testExtrapolate1bin2CorErrorsBin1()
  {
    cout << "Starting testExtrapolate1bin2CorErrorsBin1" << endl;
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh_2cerrorsbin1());
    cout << "Analysis" << endl << ana;
    cout << "Extrapolation" << endl << extrap;

    CalibrationAnalysis result (addExtrapolation(extrap, ana));

    // First bin error should remain untouched
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[0].systematicErrors.size());
    SystematicError e1(result.bins[0].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("err"), e1.name);
    CPPUNIT_ASSERT_EQUAL(double(0.1), e1.value);

    // Extrapolated bins have only one error
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[1].systematicErrors.size());
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e2.name);

    // The extrapolation is easy in the new scheme. :-)
    CPPUNIT_ASSERT_DOUBLES_EQUAL(sqrt(2)*0.1, e2.value, 0.0001);
  }

  // the bins have exactly the same error - so the extrapolated error shoudl be zero.
  void testExtrapolate1binExactlyZero()
  {
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh());
    extrap.bins[1].systematicErrors[0].value = extrap.bins[0].systematicErrors[0].value;

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(double(0.0), e2.value);
  }

  // Do the extrapolation on the low side
  void testExtrapolate1binPtLow()
  {
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_ptlow());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));

    CPPUNIT_ASSERT_EQUAL(size_t(2), result.bins.size());
    CPPUNIT_ASSERT (result.bins[1].isExtended);

    // First bin error should remain untouched
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[0].systematicErrors.size());
    SystematicError e1(result.bins[0].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("err"), e1.name);
    CPPUNIT_ASSERT_EQUAL(double(0.1), e1.value);

    // Extrapolated bins have only one error
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[1].systematicErrors.size());
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e2.name);

    // The extrapolation figures out the total, but will add in quad with the other errors,
    // so a funny quad subtraction occurs.
    CPPUNIT_ASSERT_EQUAL(0.1, e2.value);
  }

  // 1 bin analysis, extended on the pt side, but the sys error calls for shrinking
  // (throw)
  void testExtrapolate1binNegative()
  {
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh());
    extrap.bins[1].systematicErrors[0].value = 0.05;

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(0.1-0.05, e2.value);
  }

  // 1 bin analysis, extended on the pt side, and eta size (should throw b.c. we
  // don't know how to deal with multiple axis extensions.
  void testExtrapolate1binPtAndEta()
  {
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pteta());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // There is a gap between the last real bin and the first extrapolated bin.
  void testExtrapolate1binPtGap()
  {
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh());
    extrap.bins[1].binSpec[0].lowvalue = 105;

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // The last good bin and an extrapolated bin overlap in boundaries.
  void testExtrapolate1binPtOverlap()
  {
    CalibrationAnalysis ana (generate_1bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh());
    extrap.bins[1].binSpec[0].lowvalue = 95;

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // If this guy already has extrapolations applied to it, then fail.
  void testExtrapolateTwice()
  {
    CalibrationAnalysis ana (generate_2bin_ana());
    ana.bins[1].isExtended = true; // fake out the extrapolation having happened once already.
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // If the bins match exactly, there is no extrapolation. Shouldn't harm
  // anything.
  void testNoExtrapolation()
  {
    CalibrationAnalysis ana (generate_2bin_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));

    CPPUNIT_ASSERT_EQUAL(size_t(2), result.bins.size());
    CPPUNIT_ASSERT (!result.bins[0].isExtended);
    CPPUNIT_ASSERT (!result.bins[1].isExtended);

    // First bin error should remain untouched
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[0].systematicErrors.size());
    SystematicError e1(result.bins[0].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("err"), e1.name);
    CPPUNIT_ASSERT_EQUAL(double(0.1), e1.value);

    // Extrapolated bins have only one error
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[1].systematicErrors.size());
    SystematicError e2(result.bins[1].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("err"), e2.name);
    CPPUNIT_ASSERT_EQUAL(double(0.1), e2.value);
  }

  // Make sure that extrapolation happens when we have two bins in eta the match our
  // extrapolation.
  void testExtrapolateWithMulitpleEtaBins()
  {
    cout << "Starting testExtrapolateWithMulitpleEtaBins" << endl;
    CalibrationAnalysis ana (generate_2bin_2eta_ana());
    CalibrationAnalysis extrap (generate_2bin_2eta_extrap_in_pthigh());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));

    CPPUNIT_ASSERT_EQUAL(size_t(4), result.bins.size());
    CPPUNIT_ASSERT (!result.bins[0].isExtended);
    CPPUNIT_ASSERT (!result.bins[1].isExtended);
    CPPUNIT_ASSERT (result.bins[2].isExtended);
    CPPUNIT_ASSERT (result.bins[3].isExtended);

    // Extrapolated bins have only one error
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[2].systematicErrors.size());
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[3].systematicErrors.size());

    SystematicError e1(result.bins[2].systematicErrors[0]);
    SystematicError e2(result.bins[3].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e1.name);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e2.name);
    CPPUNIT_ASSERT_EQUAL(0.1, e1.value);
    CPPUNIT_ASSERT_EQUAL(0.1, e2.value);
  }

  // The analysis has multiple bins in eta, but the extrapolation has only
  // a single bin in eta.
  void testExtrapolateWithMulitpleEtaBinsWithSingleExtrapolationBin ()
  {
    cout << "Starting testExtrapolateWithMulitpleEtaBinsWithSingleExtrapolationBin..." << endl;

    CalibrationAnalysis ana(generate_2bin_2eta_ana());
    CalibrationAnalysis extrap (generate_2bin_extrap_in_pthigh());
    extrap.bins[0].binSpec[1].highvalue = 4.0;
    extrap.bins[1].binSpec[1].highvalue = 4.0;

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // Single bin in eta, but the extrapolation has multiple. Since the
  // multiple will not match exactly, this should fail.
  void testExtrapolationWithSingleEtaBinWithMultipleExtrapolation()
  {
    cout << "Starting testExtrapolationWithSingleEtaBinWithMultipleExtrapolation..." << endl;
    CalibrationAnalysis ana(generate_1bin_ana());
    ana.bins[0].binSpec[1].highvalue = 4.0;
    CalibrationAnalysis extrap (generate_2bin_2eta_extrap_in_pthigh());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // First bin of extrapolation correctly matches with the analysis. But after that
  // things go to a different binning in eta.
  void testExtrapolationWithSingleEtaBinWithMultipleSecondLevelExtrapolation()
  {
    cout << "Starting testExtrapolationWithSingleEtaBinWithMultipleSecondLevelExtrapolation" << endl;
    CalibrationAnalysis ana(generate_1bin_ana());
    ana.bins[0].binSpec[1].highvalue = 4.0;
    CalibrationAnalysis extrap (generate_2bin_1then2eta_extrap_in_pthigh());
    cout << "Analysis" << endl << ana;
    cout << "Extrapolation" << endl << extrap;

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
  }

  // Make sure if there are multiple extrapolation bins that overlap we don't really
  // care as long as the last one overlaps with ana before the jump off point.
  void testExtrapolate1binPtHigh2ExtBins()
  {
    cout << "Starting testExtrapolate1binPtHigh2ExtBins" << endl;
    CalibrationAnalysis ana(generate_1bin_ana());
    CalibrationAnalysis extrap (generate_3bin_extrap_in_pthigh());

    CalibrationAnalysis result (addExtrapolation(extrap, ana));
    CPPUNIT_ASSERT_EQUAL(size_t(3), result.bins.size());
    CPPUNIT_ASSERT (result.bins[2].isExtended);

    CPPUNIT_ASSERT_EQUAL(size_t(1), result.bins[2].systematicErrors.size());
    SystematicError e2(result.bins[2].systematicErrors[0]);
    CPPUNIT_ASSERT_EQUAL(string("extrapolated"), e2.name);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3, e2.value, 0.001); // 0.4 - 0.1
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(ExtrapolationToolsTest);

#ifdef ROOTCORE
// The common atlas test driver
#include <TestPolicy/CppUnit_testdriver.cxx>
#endif
