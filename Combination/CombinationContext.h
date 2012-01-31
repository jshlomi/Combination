///
/// The context for a combinatoin of several different measurements of the same thing (or things),
/// along with possibly shared systematic errors.
///
#ifndef COMBINATION_CombinationContext
#define COMBINATION_CombinationContext

#include "Combination/RooRealVarCache.h"

#include <string>
#include <vector>
#include <map>

class RooRealVar;

namespace BTagCombination {

  class Measurement;
  class CombinationContext
  {
  public:
    // Results of the fit for a particular measurement
    struct FitResult
    {
      double centralValue;
      double statisticalError;

      std::map<std::string, double> sysErrors;
    };

  public:
    /// Create/Destroy a new context. This will contain the common
    /// data to do a fit to mutliple measurements.
    CombinationContext(void);
    ~CombinationContext(void);

    /// Create a new measurement. You are trying to measure "what", and with
    /// this particular measurement you found a value "value", and statistical
    /// error "statError". Min and Max values are the min and max values of 'what'.
    /// You can name your measurement or let the code choose a generic name for you.
    /// Names must be unique!
    Measurement *AddMeasurement (const std::string &measurementName, const std::string &what, const double minValue, const double maxValue,
				 const double value, const double statError);
    Measurement *AddMeasurement (const std::string &what, const double minValue, const double maxValue,
				 const double value, const double statError);

    /// Fit all the measurements that we've asked for, and return results for each measurement done.
    std::map<std::string, FitResult> Fit(void);

    /// Turn on/off production of plots. Plots are expensive!
    inline void setDoPlots(bool v = false) { _doPlots = v;}

  private:
    /// Keep track of all the measurements.
    RooRealVarCache _whatMeasurements;

    /// Keep track fo all the systematic errors between the various measurements.
    RooRealVarCache _systematicErrors;

    /// Keep a list of all measurements
    std::vector<Measurement*> _measurements;

    /// Should we make plots as a diagnostic output?
    bool _doPlots;
  };
}

#endif

